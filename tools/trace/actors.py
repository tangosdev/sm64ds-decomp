"""actorcam: live actor-list heartbeat over the melonDS/DeSmuME GDB stub.

Walks the game's behavior processing list in emulator RAM and prints every live
actor: address, actor ID (joined against the community ActorList.h enum), class
vtable, its Behavior/destructor symbols from our config, and world position.
Snapshots diff against the previous one, so "what just spawned/despawned when
the star blew up" is a single keypress. Every snapshot is appended to a JSONL
session log under traces/actors/ for later joining/naming work.

Runtime layout (EU ROM, from reference/DynamicAllocationDecomp symbols.x +
include/Actor/ActorBase.h, credit CREDITS.md; vtable slot semantics validated
in tools/actor_names.py against byte-matched code, list layout corroborated by
matched src/_ZN5ActorC2Ev.cpp / _ZN5Actor4NextEPKS_):

  FIRST_ACTOR_LIST_NODE @ 0x0209b468 -> Actor::ListNode*  (global Actor list)
  Actor::ListNode (embedded at actor+0x50): +0 prev, +4 next, +8 actor(==this)
  ActorBase: +0 vtable, +4 u32 uniqueID, +8 u32 param1, +0xc u16 actorID,
             +0xe u8 aliveState (1=alive 2=dead)
  Actor:     +0x5c Vector3 pos (fix12), +0xb0 u32 flags
  vtable:    slot 6 = Behavior, slots 16/17 = D1/D0 destructors
  LOADED_LEVEL_OVL_ID @ 0x02092130

melonDS stub ground rules (hard-learned):
  - melonDS.toml needs [Gdb] Enabled=true, [Gdb.ARM9] Port/BreakOnStartup=true,
    [JIT] Enable=false. With BreakOnStartup the game waits for us at power-on;
    we auto-continue on attach.
  - EVERY stub reply must be ACKed ('+') or the stub wedges forever (rsp.py
    does this; never talk to the socket by hand).
  - Memory reads work fine WHILE THE GAME RUNS - never send 0x03 interrupts.

Usage (start melonDS with the GDB stub on first, then):
  python tools/trace/actors.py             # interactive: Enter=snapshot,
                                           #   any text = snapshot tagged with
                                           #   that label, q = quit
  python tools/trace/actors.py --once      # one snapshot and exit
  python tools/trace/actors.py --watch 2   # snapshot every 2 seconds
  python tools/trace/actors.py --port 3333 --host 127.0.0.1

The emulator keeps running between snapshots; each snapshot halts the core for
a few milliseconds of reads and resumes it.
"""
import argparse
import json
import pathlib
import re
import sys
import time

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from rsp import RspClient, RspError
import symindex

REPO = pathlib.Path(__file__).resolve().parents[2]

FIRST_ACTOR_LIST_NODE = 0x0209B468
LOADED_LEVEL_OVL_ID = 0x02092130
NODE_PREV, NODE_NEXT, NODE_ACTOR = 0x0, 0x4, 0x8
ACTOR_VTABLE, ACTOR_UID, ACTOR_PARAM1, ACTOR_ID, ACTOR_ALIVE = 0x0, 0x4, 0x8, 0xC, 0xE
ACTOR_POS = 0x5C
VT_SLOT_BEHAVIOR = 6
VT_SLOTS_READ = 18          # covers Behavior + both destructors
RAM_LO, RAM_HI = 0x02000000, 0x02400000
MAX_NODES = 1024

ACTORLIST_H = REPO / "reference" / "DynamicAllocationDecomp" / "include" / "List" / "ActorList.h"


def load_actor_names():
    """{actor_id: EnumName} from the community headers; empty dict if absent."""
    names = {}
    if ACTORLIST_H.is_file():
        for m in re.finditer(r"^\s*(\w+?)_ACTOR_ID\s*=\s*(\d+),",
                             ACTORLIST_H.read_text(encoding="utf-8"), re.M):
            names[int(m.group(2))] = m.group(1)
    return names


def u32(b, off):
    return int.from_bytes(b[off:off + 4], "little")


def u16(b, off):
    return int.from_bytes(b[off:off + 2], "little")


def s32(v):
    return v - 0x100000000 if v & 0x80000000 else v


def in_ram(p):
    return RAM_LO <= p < RAM_HI


class VtableCache:
    """vtable addr -> {'behavior': name|None, 'dtor': name|None}; reads each
    unique vtable once per session."""

    def __init__(self, cli, syms):
        self.cli = cli
        self.syms = syms
        self.cache = {}

    def _resolve(self, addr):
        """Overlay-aliasing-aware resolve. ~100 level/actor overlays share the
        same VA range, so a bare range hit can name a function from an overlay
        that is not even loaded. Return the unique name when only one symbol
        range contains addr; otherwise join all candidates with '|' and a '?'
        so the ambiguity is visible instead of silently wrong."""
        hits = sorted({name for lo, hi, name in self.syms.ranges if lo <= addr < hi})
        if not hits:
            return None
        if len(hits) == 1:
            return hits[0]
        return "?" + "|".join(hits[:4]) + ("|..." if len(hits) > 4 else "")

    def info(self, vt):
        if vt in self.cache:
            return self.cache[vt]
        out = {"behavior": None, "behavior_addr": None, "dtor": None}
        try:
            raw = self.cli.read_mem(vt, VT_SLOTS_READ * 4)
            beh = u32(raw, VT_SLOT_BEHAVIOR * 4)
            d1 = u32(raw, 16 * 4)
            if in_ram(beh):
                out["behavior_addr"] = beh
                out["behavior"] = self._resolve(beh)
            if in_ram(d1):
                out["dtor"] = self._resolve(d1)
        except RspError:
            pass
        self.cache[vt] = out
        return out


def walk_actors(cli, vtcache):
    """Walk the global Actor list and return actor dicts.

    Reads happen WHILE THE TARGET RUNS (verified stable on melonDS 1.1); a
    snapshot is very slightly racy against a mid-frame list mutation, and the
    in_ram()/cycle guards turn that into a truncated snapshot, not a crash."""
    head = u32(cli.read_mem(FIRST_ACTOR_LIST_NODE, 4), 0)
    actors, seen, node = [], set(), head
    while in_ram(node) and node not in seen and len(seen) < MAX_NODES:
        seen.add(node)
        nb = cli.read_mem(node, 0xC)
        aptr = u32(nb, NODE_ACTOR)
        if in_ram(aptr):
            ab = cli.read_mem(aptr, 0x10)
            pos = None
            try:
                pb = cli.read_mem(aptr + ACTOR_POS, 12)
                pos = [round(s32(u32(pb, i)) / 4096.0, 1) for i in (0, 4, 8)]
            except RspError:
                pass
            vt = u32(ab, ACTOR_VTABLE)
            actors.append({
                "addr": aptr,
                "vtable": vt,
                "uid": u32(ab, ACTOR_UID),
                "param1": u32(ab, ACTOR_PARAM1),
                "id": u16(ab, ACTOR_ID),
                "alive": ab[ACTOR_ALIVE],
                "pos": pos,
                **vtcache.info(vt),
            })
        node = u32(nb, NODE_NEXT)
    return actors


def fmt_row(a, names):
    name = names.get(a["id"], "?")
    beh = a["behavior"] or (f"0x{a['behavior_addr']:08x}" if a["behavior_addr"] else "-")
    pos = "({:>8} {:>8} {:>8})".format(*a["pos"]) if a["pos"] else "-"
    dead = "" if a.get("alive", 1) == 1 else " DEAD"
    return (f"  {a['addr']:08x}  id={a['id']:<4} {name:<28} uid={a['uid']:<6} "
            f"vt={a['vtable']:08x}  {pos}  beh={beh}{dead}")


def print_snapshot(actors, prev, names, mark=None, level_ovl=None):
    tag = f"  [{mark}]" if mark else ""
    lvl = f"  level_ovl={level_ovl}" if level_ovl is not None else ""
    print(f"\n=== {time.strftime('%H:%M:%S')}  {len(actors)} live actors{lvl}{tag}")
    for a in actors:
        print(fmt_row(a, names))
    if prev is not None:
        prev_uids = {a["uid"]: a for a in prev}
        cur_uids = {a["uid"]: a for a in actors}
        spawned = [a for u, a in cur_uids.items() if u not in prev_uids]
        gone = [a for u, a in prev_uids.items() if u not in cur_uids]
        for a in spawned:
            print("  ++ SPAWNED  " + fmt_row(a, names).strip())
        for a in gone:
            print("  -- GONE     " + fmt_row(a, names).strip())
        if not spawned and not gone:
            print("  (no spawn/despawn since last snapshot)")


def main():
    ap = argparse.ArgumentParser(description="live actor-list heartbeat (melonDS/DeSmuME GDB stub)")
    ap.add_argument("--host", default="127.0.0.1")
    ap.add_argument("--port", type=int, default=3333)
    ap.add_argument("--once", action="store_true", help="one snapshot, then exit")
    ap.add_argument("--watch", type=float, metavar="SEC", help="snapshot every SEC seconds")
    args = ap.parse_args()

    names = load_actor_names()
    if not names:
        print("[warn] reference/DynamicAllocationDecomp missing - actor IDs will be numeric only")
    syms = symindex.get()

    outdir = REPO / "traces" / "actors"
    outdir.mkdir(parents=True, exist_ok=True)
    log = outdir / time.strftime("session_%Y%m%d_%H%M%S.jsonl")

    print(f"[*] connecting to {args.host}:{args.port} (start melonDS + ROM first) ...")
    cli = RspClient(args.host, args.port)
    cli.connect()
    # Probe the stub with a harmless read. The melonDS 1.1 stub serves exactly
    # ONE client per emulator launch: if a previous client (even a crashed one)
    # already attached, the socket still accepts but nothing answers.
    try:
        cli.read_mem(FIRST_ACTOR_LIST_NODE, 4)
    except (RspError, OSError, TimeoutError):
        print("[!] stub connected but not answering. The melonDS GDB stub only\n"
              "    serves one client per emulator launch - close and restart\n"
              "    melonDS, then run this again (it releases the halted boot).",
              file=sys.stderr)
        cli.close()
        return
    # With BreakOnStartup=true the game powers on halted, waiting for us.
    # `c` is harmless if it is already running.
    cli.cont()
    print("[*] attached; game running.")
    print(f"[*] logging to {log.relative_to(REPO)}")
    vtcache = VtableCache(cli, syms)

    def snapshot(prev, mark=None):
        level_ovl = None
        try:
            level_ovl = u32(cli.read_mem(LOADED_LEVEL_OVL_ID, 4), 0)
        except RspError:
            pass
        actors = walk_actors(cli, vtcache)
        print_snapshot(actors, prev, names, mark, level_ovl)
        with open(log, "a", encoding="utf-8") as f:
            f.write(json.dumps({"t": time.time(), "mark": mark,
                                "level_ovl": level_ovl, "actors": actors}) + "\n")
        return actors

    prev = None
    try:
        if args.once:
            snapshot(prev)
        elif args.watch:
            while True:
                prev = snapshot(prev)
                time.sleep(args.watch)
        else:
            print("[*] Enter = snapshot | type a label (e.g. 'star exploded') = tagged snapshot | q = quit")
            while True:
                try:
                    line = input("actorcam> ").strip()
                except EOFError:
                    break
                if line.lower() in ("q", "quit", "exit"):
                    break
                prev = snapshot(prev, mark=line or None)
    except KeyboardInterrupt:
        pass
    finally:
        cli.detach()
        cli.close()
        print("[*] detached; emulator resumed.")


if __name__ == "__main__":
    main()
