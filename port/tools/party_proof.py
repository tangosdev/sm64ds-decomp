#!/usr/bin/env python3
"""PARTY co-op proof (party slice, status/PARTY_SLICE.md, status/COOP_PARTY.md).

Two headless proofs on one build, quiet/minimized/muted through
mp2_proof.env_base (the owner's quiet rule; each run's TEMP is its own dir so
the %TEMP%/sm64ds-crashes sink stays isolated from any live-player intake):

  SINGLE INSTANCE (the render-boundary authority). One walk_window with three
  seated bodies (SM64DS_VS_PLAYERS=3, VS mode off, adventure on) where
  SM64DS_PARTY_MEMBERS="1" marks slot 1 a PARTY member and leaves slot 2 an
  adventure GHOST. In ONE run [partyprobe] asserts both treatments coexist: the
  party body renders SOLID and COLLIDES while the ghost renders TRANSLUCENT and
  is PASS-THROUGH, the local body stays solid/interactive, and the party body
  TICKS BEHAVIOR (liveness, not a frozen census).

  LIVE LOOPBACK (the session authority). Two consoles seat as a PARTY: a
  rollback session on the host's level. The parent runs NetMode rollback; the
  child ASKS lockstep and must ADOPT rollback off the parent's accept bit
  (bit 17, kAcceptRollbackBit). Both boot the HOST's level (shared world
  entry). The shipped rollback sim re-ticks every Behavior and moves the bodies
  under injected input (liveness by real movement).

  python port/tools/party_proof.py
"""
import os, re, sys, time
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import mp2_proof as M
PORT = os.path.dirname(HERE); ROOT = os.path.dirname(PORT)
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(PORT, "tools", "_party_out")
HOSTLEVEL = 2; PORTN = 44700; FRAMES = 600
FAILS = []

def ck(n, ok, d=""):
    print(("ok    " if ok else "FAIL  ") + n + (("  " + d) if d and not ok else ""))
    if not ok: FAILS.append(n)

def last(t, rx, g=1):
    m = None
    for m in re.finditer(rx, t): pass
    return m.group(g) if m else None

def single_instance():
    print("\n-- single instance: the ghost-vs-party render boundary + liveness")
    d = os.path.join(OUT, "single"); os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    e = M.env_base(ROOT, d, "party1")
    e["SM64DS_WINDOW_SELFTEST"] = "160"; e["SM64DS_LEVEL"] = "1"
    e["SM64DS_ADVENTURE"] = "1"; e["SM64DS_VS_PLAYERS"] = "3"
    e["SM64DS_PARTY_MEMBERS"] = "1"; e["SM64DS_PARTY_PROBE"] = "1"
    log = os.path.join(d, "run.log"); rc = M.run_one(EXE, d, e, log)
    t = M.text(log)
    ck("the instance exited cleanly", rc == 0, "rc=%d" % rc)
    m = re.search(r"\[partyprobe\].*=> (ALL PASS|FAIL)", t)
    ck("the render boundary + liveness pass ([partyprobe] ALL PASS)",
       bool(m) and m.group(1) == "ALL PASS", m.group(0) if m else "no [partyprobe] line")
    if m: print("      " + m.group(0))

def mk(role, netmode, slot):
    d = os.path.join(OUT, "live_" + role); os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    e = M.env_base(ROOT, d, "pty" + role[:1])
    e["SM64DS_NETMODE"] = netmode
    e["SM64DS_PARTY"] = "1"; e["SM64DS_PARTY_DIAG"] = "30"
    e["SM64DS_LEVEL"] = str(HOSTLEVEL)   # the launcher hands the joiner the HOST's level
    e["SM64DS_WINDOW_SELFTEST"] = str(FRAMES)
    e["SM64DS_COMMS_ROLE"] = role; e["SM64DS_COMMS_PORT"] = str(PORTN)
    e["SM64DS_COMMS_FANOUT"] = "1"; e["SM64DS_VS_STATE_HASH"] = "1"
    e["SM64DS_ROLLBACK_LOCALPROBE"] = "1"
    e["SM64DS_COMMS_INJECT"] = ("key=0x0040,toggle=45,key2=0x0080" if role == "parent"
                                else "key=0x0080,toggle=45,key2=0x0040")
    if slot: e["SM64DS_COMMS_SLOT"] = str(slot)
    return d, e

def samples(t):
    xs = []
    for m in re.finditer(r"\[partydiag\][^\n]*slot(\d+)\(LOCAL\)[^\n]*pos=\((-?\d+),(-?\d+),(-?\d+)\) anim=(-?\d+)", t):
        xs.append((int(m.group(2)), int(m.group(4)), int(m.group(5))))
    return xs

def live_loopback():
    print("\n-- live loopback: two consoles seat as a party (rollback session)")
    os.makedirs(OUT, exist_ok=True)
    dp, ep = mk("parent", "rollback", 0)
    dc, ec = mk("child", "lockstep", 1)   # child ASKS lockstep -> must adopt via bit 17
    lp = os.path.join(dp, "run.log"); lc = os.path.join(dc, "run.log")
    pp = M.spawn(EXE, dp, ep, lp); time.sleep(0.8); pc = M.spawn(EXE, dc, ec, lc)
    rcp = M.finish(pp, 1500); rcc = M.finish(pc, 1500)
    tp = M.text(lp); tc = M.text(lc)
    print("      rcp=%d rcc=%d" % (rcp, rcc))
    ck("parent: NetMode ROLLBACK took", "NetMode ROLLBACK" in tp)
    adopt = re.search(r"the parent runs NetMode rollback and this end had lockstep; ADOPTING", tc)
    ck("child: ADOPTS rollback via bit 17 (asked lockstep, took the parent's rollback)", bool(adopt))
    for tag, t in (("parent", tp), ("child", tc)):
        live = last(t, r"rollback: live=0x[0-9a-f]+ peak=0x([0-9a-f]+)")
        ck("%s: session live mask full (both seated)" % tag,
           live is not None and int(live, 16) == 0x3, "peak=%s" % live)
    for tag, t in (("parent", tp), ("child", tc)):
        lv = last(t, r"\[partydiag\][^\n]*mylev=(-?\d+)")
        ck("%s: booted the HOST's level %d (shared world entry)" % (tag, HOSTLEVEL),
           lv is not None and int(lv) == HOSTLEVEL, "mylev=%s" % lv)
    for tag, t in (("parent", tp), ("child", tc)):
        ck("%s: rb-local same-frame OK" % tag, last(t, r"\[rb-local\].*-> (\w+)") == "OK")
    sp, sc = samples(tp), samples(tc)
    def anim_ticked(xs): return len(xs) >= 2 and len({a for _, _, a in xs}) > 1
    def pos_delta(xs):
        if len(xs) < 2: return 0
        return (max(x for x, _, _ in xs) - min(x for x, _, _ in xs)) + \
               (max(z for _, z, _ in xs) - min(z for _, z, _ in xs))
    for tag, xs in (("parent", sp), ("child", sc)):
        ck("%s: party body TICKS BEHAVIOR (anim advances, live not frozen)" % tag, anim_ticked(xs))
    ck("the shipped sim MOVES a party body through the world (real traversal)",
       max(pos_delta(sp), pos_delta(sc)) > 4096,
       "deltas parent=%d child=%d" % (pos_delta(sp), pos_delta(sc)))
    for tag, t in (("parent", tp), ("child", tc)):
        u = last(t, r"unrecoverable=(\d+)")
        ck("%s: every rewind honoured (unrecoverable=0)" % tag, u == "0", "unrecoverable=%s" % u)

def main():
    if not os.path.exists(EXE):
        print("no walk_window.exe at %s -- build first" % EXE); return 2
    print("party_proof: exe %s  out %s" % (M.sha(EXE), OUT))
    single_instance()
    try:
        live_loopback()
    except Exception as ex:
        ck("live loopback run completed", False, repr(ex))
    print("\n%s (%d failed)" % ("ALL GREEN" if not FAILS else "FAILURES ABOVE", len(FAILS)))
    for n in FAILS: print("  FAILED: %s" % n)
    return 0 if not FAILS else 1

if __name__ == "__main__":
    sys.exit(main())
