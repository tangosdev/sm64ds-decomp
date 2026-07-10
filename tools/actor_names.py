"""Name actor classes across overlays from the ROM's own actor-spawn table.

The EU arm9 statically links ACTOR_SPAWN_TABLE at 0x02090864 (391 SpawnInfo*,
holder 0x020A4BB8 assigned at 0x0201A128). Each SpawnInfo (layout from
reference/DynamicAllocationDecomp, credit CREDITS.md):
    +0 spawnFunc  +4 s16 behavPrio  +6 s16 renderPrio  +8 flags  +c..+18 Fix12
The spawn function allocates the instance (mov r0,#size before the first bl)
and stores the class vtable (last pool literal stored to [r4]). Vtable slot
order is the ActorBase virtual order (validated on ov056 BigMovingIceBlock:
slot 6 Behavior == the byte-matched path follower; slots 16/17 = D1/D0 dtors).

Emits renames for: spawn func, SpawnInfo data, vtable data, and every vtable
slot whose target lives in the actor's own overlay (arm9 defaults are
inherited, skipped). Names: Itanium-mangled methods to match repo convention.

Conservative rules (mirrors import_symbols.py):
  - only rename func_<addr>/data_<addr> placeholders, never dsd/SDK names
  - a name already present anywhere in config is skipped (no duplicates)
  - one actor claiming a function already claimed by another actor: first
    (lowest actor id) wins, rest reported

Usage:
  python tools/actor_names.py            # dry run: writes actor_renames.tsv + report
  python tools/actor_names.py --apply    # rewrite config/**/symbols.txt in place
"""
import argparse
import collections
import pathlib
import re
import struct

REPO = pathlib.Path(__file__).resolve().parent.parent
TABLE = 0x02090864
N_ACTORS = 391
ARM9_BASE = 0x02004000
DL = REPO / "reference" / "DynamicAllocationDecomp"

SLOT_SIGS = [
    ("InitResources", "Ev"), ("BeforeInitResources", "Ev"), ("AfterInitResources", "Ej"),
    ("CleanupResources", "Ev"), ("BeforeCleanupResources", "Ev"), ("AfterCleanupResources", "Ej"),
    ("Behavior", "Ev"), ("BeforeBehavior", "Ev"), ("AfterBehavior", "Ej"),
    ("Render", "Ev"), ("BeforeRender", "Ev"), ("AfterRender", "Ej"),
    ("OnPendingDestroy", "Ev"), ("Virtual34", "Ejj"), ("Virtual38", "Ejj"),
    ("OnHeapCreated", "Ev"), ("D1", ""), ("D0", ""),
]


def load_modules():
    mods = [("arm9", ARM9_BASE, (REPO / "extracted/dsd/arm9/arm9.bin").read_bytes())]
    txt = (REPO / "extracted/dsd/arm9_overlays/overlays.yaml").read_text()
    for m in re.finditer(r"- id: (\d+)\n\s+base_address: (\d+)", txt):
        oid, base = int(m.group(1)), int(m.group(2))
        p = REPO / f"extracted/dsd/arm9_overlays/ov{oid:03d}.bin"
        if p.exists():
            mods.append((f"ov{oid:03d}", base, p.read_bytes()))
    return mods


def load_actor_names():
    names = {}
    for m in re.finditer(r"^\s*(\w+)_ACTOR_ID\s*=\s*(\d+),",
                         (DL / "include/List/ActorList.h").read_text(), re.M):
        names[int(m.group(2))] = m.group(1)
    return names


def load_header_classes():
    """PascalCase class names from the community headers, keyed by squashed lowercase."""
    out = {}
    for p in (DL / "include" / "Actors").glob("*.h"):
        out[p.stem.replace("_", "").lower()] = p.stem
    return out


def pascal_case(enum_name, header_classes):
    squash = enum_name.replace("_", "").lower()
    if squash in header_classes:
        return header_classes[squash]
    return "".join(w.capitalize() for w in enum_name.lower().split("_"))


def load_config():
    """(module -> addr -> (name, kind, file, line_idx)), all-names set, file cache"""
    syms = collections.defaultdict(dict)
    all_names = set()
    files = {}

    def load(path, module):
        if not path.exists():
            return
        lines = path.read_text().splitlines()
        files[module] = (path, lines)
        for i, line in enumerate(lines):
            m = re.match(r"(\S+) kind:(function|data|bss)\S* addr:(0x[0-9a-fA-F]+)", line)
            if m:
                syms[module][int(m.group(3), 16)] = (m.group(1), m.group(2), i)
                all_names.add(m.group(1))

    load(REPO / "config/arm9/symbols.txt", "arm9")
    for d in sorted((REPO / "config/arm9/overlays").iterdir()):
        load(d / "symbols.txt", d.name)
    return syms, all_names, files


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--apply", action="store_true")
    args = ap.parse_args()

    mods = load_modules()
    actor_names = load_actor_names()
    header_classes = load_header_classes()
    syms, all_names, files = load_config()
    arm9 = mods[0][2]

    def containing(addr):
        return [(n, b, bl) for n, b, bl in mods if b <= addr < b + len(bl)]

    def word(blob, base, addr):
        off = addr - base
        if 0 <= off <= len(blob) - 4:
            return struct.unpack_from("<I", blob, off)[0]
        return None

    def is_func(mod, addr):
        e = syms.get(mod, {}).get(addr)
        return e is not None and e[1] == "function"

    def code_ptr_ok(addr, mod, base, blob):
        return (base <= addr < base + len(blob) and is_func(mod, addr)) or \
               (ARM9_BASE <= addr < ARM9_BASE + len(arm9) and is_func("arm9", addr))

    def validate_spawninfo(mod, base, blob, ptr):
        off = ptr - base
        if not (0 <= off <= len(blob) - 0x1c):
            return None
        fn, bp, rp, flags = struct.unpack_from("<IhhI", blob, off)
        rng = struct.unpack_from("<4i", blob, off + 0xC)
        if fn & 3 or not code_ptr_ok(fn, mod, base, blob):
            return None
        if not (-2000 <= bp <= 2000 and -2000 <= rp <= 2000):
            return None
        for v in rng:
            if not (v == 0 or 0x100 <= v <= 0x10000000):
                return None
        fmod = mod if base <= fn < base + len(blob) else "arm9"
        return fn, fmod, bp, rp

    def parse_spawnfunc(fn, mod, base, blob):
        """-> (alloc_size|None, vtable_addr|None); scan first 0x50 bytes"""
        size = None
        vt = None
        off = fn - base
        code = blob[off:off + 0x50]
        last_pool_to_r4 = None
        for i in range(0, len(code) - 3, 4):
            w = struct.unpack_from("<I", code, i)[0]
            if w & 0xFFFFF000 == 0xE3A00000:  # mov r0, #imm8 ror
                imm, rot = w & 0xFF, (w >> 8) & 0xF
                size = (imm >> (2 * rot)) | (imm << (32 - 2 * rot)) & 0xFFFFFFFF
                size &= 0xFFFFFFFF
            if w & 0xFFFF0000 == 0xE59F0000:  # ldr rX,[pc,#imm]
                lit_addr = fn + i + 8 + (w & 0xFFF)
                lit = word(blob, base, lit_addr)
                rd = (w >> 12) & 0xF
                if lit is not None:
                    # remember pool loads; a following str rd,[r4] claims it
                    for j in range(i + 4, min(i + 16, len(code) - 3), 4):
                        w2 = struct.unpack_from("<I", code, j)[0]
                        if w2 == 0xE5840000 | (rd << 12):  # str rd,[r4]
                            last_pool_to_r4 = lit
        if last_pool_to_r4 is not None:
            # vtable must be 4+ consecutive code pointers
            ok = 0
            for k in range(4):
                wv = word(blob, base, last_pool_to_r4 + 4 * k)
                if wv is not None and code_ptr_ok(wv, mod, base, blob):
                    ok += 1
            if ok == 4:
                vt = last_pool_to_r4
        return size, vt

    renames = []      # (module, addr, old, new, why)
    claimed = {}      # (module, addr) -> class
    report = []
    overlay_actors = collections.defaultdict(list)

    def propose(mod, addr, new, cls, why):
        e = syms.get(mod, {}).get(addr)
        if e is None:
            report.append(f"SKIP {cls}: no symbol at {mod}:{addr:#x} ({why})")
            return
        old = e[0]
        if not re.match(r"(func|data)_(ov\d+_)?[0-9a-fA-F]{8}$", old):
            return  # already named - never clobber
        if new in all_names:
            report.append(f"SKIP {cls}: name {new} already exists ({mod}:{addr:#x})")
            return
        key = (mod, addr)
        if key in claimed:
            report.append(f"SKIP {cls}: {mod}:{addr:#x} already claimed by {claimed[key]} ({why})")
            return
        claimed[key] = cls
        all_names.add(new)
        renames.append((mod, addr, old, new, why))

    for i in range(N_ACTORS):
        ptr = word(arm9, ARM9_BASE, TABLE + 4 * i)
        if not ptr:
            continue
        enum_name = actor_names.get(i)
        if not enum_name:
            continue
        cls = pascal_case(enum_name, header_classes)
        cands = []
        for mod, base, blob in containing(ptr):
            v = validate_spawninfo(mod, base, blob, ptr)
            if v:
                cands.append((mod, base, blob) + v)
        if not cands:
            report.append(f"UNRESOLVED actor {i} {enum_name}: spawninfo {ptr:#x}")
            continue
        # prefer candidate whose spawnFunc is in the same module (overlay-owned)
        cands.sort(key=lambda c: (c[0] != c[4], c[0]))
        if len(cands) > 1:
            report.append(f"AMBIG actor {i} {enum_name}: {[c[0] for c in cands]} -> {cands[0][0]}")
        mod, base, blob, fn, fmod, bp, rp = cands[0]
        overlay_actors[fmod].append((i, enum_name, cls))

        info_mod = mod
        propose(info_mod, ptr, f"{cls}_SpawnInfo", cls, "spawninfo")
        propose(fmod, fn, f"{cls}_Spawn", cls, "spawnfunc")

        fblob, fbase = (blob, base) if fmod == mod else (arm9, ARM9_BASE)
        size, vt = parse_spawnfunc(fn, fmod, fbase, fblob)
        if vt is None:
            report.append(f"NOVTABLE actor {i} {cls} (alloc={hex(size) if size else '?'})")
            continue
        propose(fmod, vt, f"_ZTV{len(cls)}{cls}", cls, f"vtable alloc={hex(size) if size else '?'}")
        for slot, (mname, sig) in enumerate(SLOT_SIGS):
            tgt = word(fblob, fbase, vt + 4 * slot)
            if tgt is None or not (fbase <= tgt < fbase + len(fblob)):
                continue  # inherited default in arm9 (or bad) - skip
            if fmod == "arm9":
                continue  # arm9-owned actors: too risky to attribute shared defaults
            if mname in ("D1", "D0"):
                new = f"_ZN{len(cls)}{cls}{mname}Ev"
            else:
                new = f"_ZN{len(cls)}{cls}{len(mname)}{mname}{sig}"
            propose(fmod, tgt, new, cls, f"vtable slot {slot}")

    out_dir = REPO / "symbols"
    out_dir.mkdir(exist_ok=True)
    tsv = out_dir / "actor_renames.tsv"
    with tsv.open("w") as f:
        f.write("module\taddr\told\tnew\twhy\n")
        for mod, addr, old, new, why in sorted(renames):
            f.write(f"{mod}\t{addr:#010x}\t{old}\t{new}\t{why}\n")
    rep = out_dir / "actor_renames_report.txt"
    rep.write_text("\n".join(report) + "\n")

    md = ["# Overlay -> actor map (from ACTOR_SPAWN_TABLE @ 0x02090864)\n"]
    for mod in sorted(overlay_actors):
        acts = ", ".join(f"{n}({i})" for i, n, c in overlay_actors[mod])
        md.append(f"- **{mod}**: {acts}")
    (out_dir / "overlay_actors.md").write_text("\n".join(md) + "\n")

    print(f"renames={len(renames)} report_lines={len(report)}")
    print(f"modules touched: {len({r[0] for r in renames})}")
    print(f"-> {tsv}\n-> {rep}\n-> {out_dir / 'overlay_actors.md'}")

    if args.apply:
        by_mod = collections.defaultdict(list)
        for mod, addr, old, new, why in renames:
            by_mod[mod].append((old, new))
        for mod, pairs in by_mod.items():
            path, lines = files[mod]
            repl = dict(pairs)
            out = []
            for line in lines:
                first = line.split(" ", 1)[0]
                if first in repl:
                    line = line.replace(first + " ", repl[first] + " ", 1)
                out.append(line)
            path.write_text("\n".join(out) + "\n")
        print(f"APPLIED to {len(by_mod)} symbols.txt files")

        # rewrite src references (extern decls + call sites); byte-safe: the
        # symbol still resolves to the same address via config
        repl_all = {old: new for _, _, old, new, _ in renames}
        pat = re.compile(r"\b(?:func|data)_(?:ov\d+_)?[0-9a-fA-F]{8}\b")
        touched = 0
        for p in (REPO / "src").glob("*.c*"):
            t = p.read_text(errors="ignore")
            t2 = pat.sub(lambda m: repl_all.get(m.group(0), m.group(0)), t)
            if t2 != t:
                p.write_text(t2)
                touched += 1
        print(f"REWROTE references in {touched} src files")

        # keep the src/<symbol>.c invariant: rename files whose stem was renamed
        moved = 0
        for p in list((REPO / "src").glob("*.c*")):
            new = repl_all.get(p.stem)
            if new:
                p.rename(p.with_name(new + "".join(p.suffixes)))
                moved += 1
        print(f"RENAMED {moved} src files")

        # sync the local (gitignored) progress ledgers so linkcheck/progress
        # keep resolving; contributors run this once after pulling the renames
        for ledger in (REPO / "progress/matched.jsonl", REPO / "progress/nonmatching.jsonl"):
            if not ledger.exists():
                continue
            t = ledger.read_text()
            t2 = pat.sub(lambda m: repl_all.get(m.group(0), m.group(0)), t)
            if t2 != t:
                ledger.write_text(t2)
                print(f"SYNCED {ledger.name}")


if __name__ == "__main__":
    main()
