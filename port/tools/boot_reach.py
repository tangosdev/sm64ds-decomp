#!/usr/bin/env python3
"""What does the ROM's own boot chain reach, and what would seating a step link?

WHY THIS EXISTS. The port boots through tests/walk_window.cpp's main(), which
stands in for the ROM's Entry and main(). Every matched translation unit that
is reachable ONLY through that chain -- the OS, the IRQ and CP15 primitives,
the thread system, the file-system and overlay loaders, the wireless manager,
the sound bring-up, the scene boot -- was therefore unlinked, not because it
could not build but because nothing the port ran ever called it. That was 503
TUs when this file was written.

Seating one of those boot steps is cheap to do and expensive to do BLIND: the
question that decides it is not "does it compile" but "what does the ROM's own
reference graph pull in behind it, and which of those bodies would actually
RUN on a host". This answers both off config/arm9/relocs.txt and
config/arm9/symbols.txt, with no build and no link.

    python port/tools/boot_reach.py --root C:/tmp/l1-boot census
        Group a TU list by the boot-spine step that reaches it and by kind:
          a  hardware primitive no host can run (CP15 cache/MPU, ARM context
             switch, BIOS SWI, hand-asm)
          b  SDK software driving hardware through registers the ntr layer
             models or could model (timers, VBlank, VRAM banks, PXI, wireless)
          c  pure software (decompressors, loaders, arena and thread
             bookkeeping, scene boot)

    python port/tools/boot_reach.py --root . slice --seed func_0201a5cc ...
        The matched TUs the ROM's call graph reaches from those seeds, minus
        the ones a host file already defines (those would be LNK2005) and minus
        the hand-asm primitives MSVC cannot assemble. That list IS the slice
        file; the skipped ones are printed so the gap stays visible.

    python port/tools/boot_reach.py --root . hazard --seed func_02060890
        For each seed, which functions in its closure touch an address NO
        ntr::kRegions entry maps. This is the check that kept the gate-214 seat
        off func_02058ec8 and func_02059594: both reach func_02058764, which
        reads the Slot-2 magic at 0x08000000, and that address is in no region,
        so the boot would have faulted on the first call rather than linked
        eighty more files.

THE ONE THING IT CANNOT TELL YOU is whether a body that links will also
BEHAVE. A ROM function that writes past the end of an undersized hosted global
looks identical here to one that does not; func_0201a9fc writes an s64 at
data_0209d574 + 0x38 and the host object is 32 bytes, which only reading the
host definition finds. Size the globals by ROM span before seating a writer.
"""

import argparse
import bisect
import collections
import glob
import json
import os
import re
import sys

SYM = re.compile(r"(\S+) kind:(\w+)(?:\(([^)]*)\))? addr:0x([0-9a-f]+)")
REL = re.compile(r"from:0x([0-9a-f]+) kind:(\w+) to:0x([0-9a-f]+) module:(\S+)")
MMIO = re.compile(r"0x0?4[0-9a-fA-F]{6}\b")
VRAMPAL = re.compile(r"0x0?[567][0-9a-fA-F]{6}\b")
ASMBLK = re.compile(r"(^|\s)(asm\s+\w|__asm|\basm\s*\{)")
ADDRLIT = re.compile(r"0x0?[0-9a-fA-F]{7,8}\b")

# ntr::kRegions, from port/ntr/include/ntr/mmio.h. An address literal outside
# every one of these has no memory behind it in the 32-bit process.
NTR_REGIONS = [
    (0x02000000, 0x02400000),   # main RAM (DTCM at 0x023c0000 lives inside it)
    (0x027FF000, 0x02800000),   # the shared system block
    (0x04000000, 0x04002000),   # 2D/3D/system registers
    (0x05000000, 0x05000800),   # palettes
    (0x06000000, 0x06900000),   # all VRAM banks
    (0x07000000, 0x07000800),   # OAM
]

# The ROM boot spine, in the order src/Entry.c and src/main.c run it.
SPINE = [
    ("E1 CP15::SystemSetup",       "func_020049f0"),
    ("E2 autoload-seg-init",       "func_020048d8"),
    ("E3 seg-init-2",              "func_0200497c"),
    ("E4 MultiStore_Int clears",   "func_0205a47c"),
    ("E5 itcm-init",               "func_01ffafd4"),
    ("E6 pre-main init",           "func_02019780"),
    ("E7 func_02072f94",           "func_02072f94"),
    ("M1 OS_InitTick",             "func_02059788"),
    ("M2 alarms",                  "func_02059bc0"),
    ("M3 main-thread record",      "func_02059578"),
    ("M4 Heap::SetNodeID",         "_ZN4Heap9SetNodeIDEj"),
    ("M5 game init",               "func_0201a054"),
    ("M6 main loop",               "func_020197b8"),
]
BIG = {"func_02019780": "E6", "func_0201a054": "M5", "func_020197b8": "M6"}

NAME_A = re.compile(r"^(_ZN4CP15|ARMSaveContext|ARMRestoreContext|ARMProcessorMode"
                    r"|CpuSet|CpuFastSet|WaitByLoop|SoftReset|Halt|IntrWait"
                    r"|VBlankIntrWait|Entry)")
NAME_A2 = re.compile(r"^_ZN3IRQ(6Enable|7Disable|7Restore|10DisableAll|10RestoreAll)")
NAME_B = re.compile(r"^(WM_|Wireless_|_ZN3IRQ13VBlankHandler|_ZN3IRQ21GameCardIREQMC"
                    r"|_ZN3IRQ24IPCRxFifoNotEmpty|_ZN3OAM5Flush)")
NAME_C = re.compile(r"^(DecompressLZ16|MultiCopy|MultiStore|CpuCopy8|GetSceneOverlayID"
                    r"|LoadOverlay|UnloadOverlay|FS_LoadOverlay|OS_SleepThread"
                    r"|OS_WakeupThread|OS_GetInitArenaLo|_ZN5Scene18PrepareToSpawnBoot"
                    r"|_ZN4Heap|_ZN5Sound|_ZN4cstd)")


class Graph:
    """config/arm9 as a call graph: symbol -> the symbols its code relocates to."""

    def __init__(self, root):
        self.root = root
        self.name2 = {}
        self.mods = {}
        self._load_syms("main", os.path.join(root, "config/arm9/symbols.txt"))
        for d in sorted(glob.glob(os.path.join(root, "config/arm9/overlays/ov*"))):
            p = os.path.join(d, "symbols.txt")
            if os.path.exists(p):
                self._load_syms(os.path.basename(d), p)
        self.fwd = collections.defaultdict(set)
        self._load_relocs("main", os.path.join(root, "config/arm9/relocs.txt"))
        for d in sorted(glob.glob(os.path.join(root, "config/arm9/overlays/ov*"))):
            p = os.path.join(d, "relocs.txt")
            if os.path.exists(p):
                self._load_relocs(os.path.basename(d), p)

    def _load_syms(self, mod, path):
        lst = []
        for line in open(path, errors="replace"):
            m = SYM.match(line)
            if not m:
                continue
            name, attrs, addr = m.group(1), m.group(3) or "", int(m.group(4), 16)
            sz = re.search(r"size=0x([0-9a-f]+)", attrs)
            sz = int(sz.group(1), 16) if sz else 0
            self.name2.setdefault(name, (mod, addr, sz))
            lst.append((addr, sz, name))
        lst.sort()
        self.mods[mod] = ([a for a, _, _ in lst], lst)

    def _containing(self, mod, addr):
        if mod not in self.mods:
            return None
        addrs, lst = self.mods[mod]
        i = bisect.bisect_right(addrs, addr) - 1
        if i < 0:
            return None
        a, sz, name = lst[i]
        if sz and addr >= a + sz:
            return None
        return name

    def _load_relocs(self, mod, path):
        for line in open(path, errors="replace"):
            m = REL.match(line)
            if not m:
                continue
            fr, to, tomod = int(m.group(1), 16), int(m.group(3), 16), m.group(4)
            if tomod.startswith("overlay"):
                tomod = "ov%03d" % int(re.sub(r"\D", "", tomod) or 0)
            if tomod != "main" and not tomod.startswith("ov"):
                continue
            tn = self._containing(tomod, to)
            fn = self._containing(mod, fr)
            if tn and fn and tn != fn:
                self.fwd[fn].add(tn)

    def closure(self, seeds, ban=()):
        ban = set(ban)
        seen, stack = set(), list(seeds)
        while stack:
            x = stack.pop()
            if x in seen or x in ban:
                continue
            seen.add(x)
            stack.extend(self.fwd.get(x, ()))
        return seen


def read_tu_list(path):
    """A TSV whose first column is a TU stem and (optionally) second a src path."""
    rows = []
    for line in open(path, errors="replace"):
        if line.startswith("#"):
            continue
        p = line.rstrip("\n").split("\t")
        if p and p[0]:
            rows.append(p)
    return rows


def src_path(root, tu, given=None):
    if given and os.path.exists(os.path.join(root, given)):
        return given
    for ext in (".c", ".cpp"):
        q = "src/" + tu + ext
        if os.path.exists(os.path.join(root, q)):
            return q
    return None


def scan_src(root, path):
    try:
        txt = open(os.path.join(root, path), errors="replace").read()
    except OSError:
        return None
    head = txt[:2000]
    return {
        "asm": bool(ASMBLK.search(txt)) or "HAND-ASM" in head,
        "mm": sorted(set(MMIO.findall(txt))),
        "vr": sorted(set(VRAMPAL.findall(txt))),
        "nm": "NONMATCHING" in head,
        "stub": "recovered from vtable slot identity" in head,
        "text": txt,
    }


def kind_of(tu, f):
    if NAME_C.match(tu):
        return "c", "pure software (SDK bookkeeping / decompressor / loader)"
    if NAME_A.match(tu) or NAME_A2.match(tu):
        return "a", "hardware primitive: CP15 / ARM mode / BIOS SWI"
    if NAME_B.match(tu):
        return "b", "SDK hardware driver (wireless / IRQ handler / OAM DMA)"
    if not f:
        return "c", "pure software (no src found)"
    if f["asm"]:
        return "a", "hand-asm primitive (src carries the banner)"
    if f["mm"]:
        return "b", "writes DS I/O registers " + ",".join(f["mm"][:4])
    if f["vr"]:
        return "b", "writes VRAM/palette/OAM " + ",".join(f["vr"][:3])
    return "c", "pure software"


def host_definitions(root, names):
    """Which of `names` does a host file under port/ already DEFINE? Those are
    the LNK2005 candidates -- a slice must not seat them."""
    hits = collections.defaultdict(set)
    files = []
    for r in ("port/hal", "port/unmatched", "port/ntr", "port/tests"):
        for dp, _, fn in os.walk(os.path.join(root, r)):
            files += [os.path.join(dp, f) for f in fn
                      if f.endswith((".cpp", ".c", ".inc", ".h"))]
    call = re.compile(r'(?:^|[\s*&])([A-Za-z_][A-Za-z0-9_]*)\s*\(', re.M)
    for p in files:
        try:
            txt = open(p, errors="replace").read()
        except OSError:
            continue
        for m in call.finditer(txt):
            n = m.group(1)
            if n not in names:
                continue
            # walk to the matching close paren, then look for a body
            i, depth = m.end() - 1, 0
            while i < len(txt):
                if txt[i] == "(":
                    depth += 1
                elif txt[i] == ")":
                    depth -= 1
                    if depth == 0:
                        break
                i += 1
            j = i + 1
            while j < len(txt) and txt[j] in " \t\r\n":
                j += 1
            if j < len(txt) and txt[j] == "{":
                hits[n].add(os.path.relpath(p, root))
    return hits


def mapped(addr):
    return any(lo <= addr < hi for lo, hi in NTR_REGIONS)


def cmd_census(args, g, rows):
    root = args.root
    tus = {p[0] for p in rows}
    addr = {p[0]: (p[3] if len(p) > 3 else "0x%08x" % g.name2.get(p[0], ("", 0, 0))[1])
            for p in rows}
    cl = {lbl: g.closure([fn]) for lbl, fn in SPINE}
    sub = []
    for big, tag in BIG.items():
        for k in sorted(g.fwd.get(big, ())):
            sub.append(("%s/%s" % (tag, k), g.closure([k])))
    kc, sc, s2c = collections.Counter(), collections.Counter(), collections.Counter()
    out = []
    for p in sorted(rows, key=lambda r: addr[r[0]]):
        tu = p[0]
        hits = [lbl for lbl, _ in SPINE if tu in cl[lbl]]
        sr = hits[0] if hits else "unreached"
        h2 = [lbl for lbl, c in sub if tu in c]
        sr2 = h2[0] if h2 else "-"
        f = scan_src(root, src_path(root, tu, p[1] if len(p) > 1 else None))
        k, note = kind_of(tu, f)
        if f and f["nm"]:
            note += " [NONMATCHING-banner]"
        if f and f["stub"]:
            note += " [INFERRED-STUB]"
        out.append((tu, addr[tu], sr, sr2, k, note))
        kc[k] += 1
        sc[sr] += 1
        s2c[sr2] += 1
    if args.out:
        with open(args.out, "w") as fh:
            fh.write("# boot_reach.py census: matched TUs reachable only through the "
                     "ROM's Entry/main\n")
            fh.write("# sub-root = the earliest boot-spine step whose call closure "
                     "contains the TU (E*=Entry, M*=main)\n")
            fh.write("# kind: a=hardware primitive no host can run  b=SDK over "
                     "registers ntr models or could model  c=pure software\n")
            fh.write("tu\taddr\tsub-root\tsub-root-2\tkind\tnote\n")
            for r in out:
                fh.write("\t".join(r) + "\n")
        print("wrote", args.out)
    print("KIND:", dict(kc))
    print("SUB-ROOT:")
    for lbl, _ in SPINE:
        if sc[lbl]:
            print("  %-28s %4d" % (lbl, sc[lbl]))
    if sc["unreached"]:
        print("  %-28s %4d" % ("unreached", sc["unreached"]))
    print("SECOND LEVEL (top 15):")
    for k, n in s2c.most_common(15):
        print("  %-44s %4d" % (k, n))


def cmd_slice(args, g, rows):
    root = args.root
    tus = {p[0] for p in rows}
    srcs = {p[0]: src_path(root, p[0], p[1] if len(p) > 1 else None) for p in rows}
    hostdef = host_definitions(root, tus)
    c = g.closure(args.seed, args.ban)
    take, skip_host, skip_asm = [], [], []
    for t in sorted(tus):
        if t not in c:
            continue
        if t in hostdef:
            skip_host.append(t)
            continue
        f = scan_src(root, srcs[t])
        if f and f["asm"]:
            skip_asm.append(t)
            continue
        if srcs[t]:
            take.append(t)
    print("closure %d | of-list %d | TAKE %d | skip(host defines it) %d | "
          "skip(hand-asm) %d"
          % (len(c), sum(1 for t in tus if t in c), len(take),
             len(skip_host), len(skip_asm)))
    print("SKIP because a host file already defines it (would be LNK2005):")
    for t in skip_host:
        print("    %-42s %s" % (t, ",".join(sorted(hostdef[t]))))
    print("SKIP because it is hand-asm (needs a PORT_HOST_ABI stand-in if a "
          "seated TU calls it):")
    for t in skip_asm:
        print("    %s" % t)
    wants = collections.defaultdict(list)
    tk = set(take)
    for t in take:
        for y in g.fwd.get(t, ()):
            if y in tus and y not in tk and y not in hostdef:
                wants[y].append(t)
    if wants:
        print("STAND-INS THE SEATED SET WILL CALL:")
        for y in sorted(wants):
            print("    %-42s <- %s" % (y, ",".join(wants[y][:4])))
    if args.out:
        with open(args.out, "w") as fh:
            for t in take:
                fh.write(srcs[t] + "\n")
        print("wrote", args.out, len(take), "lines")


def cmd_hazard(args, g, rows):
    root = args.root
    srcs = {p[0]: src_path(root, p[0], p[1] if len(p) > 1 else None) for p in rows}
    for seed in args.seed:
        bad = []
        for t in sorted(g.closure([seed], args.ban)):
            p = srcs.get(t) or src_path(root, t)
            if not p:
                continue
            f = scan_src(root, p)
            if not f:
                continue
            hit = []
            for lit in set(ADDRLIT.findall(f["text"])):
                a = int(lit, 16)
                if a < 0x01000000 or a >= 0x09000000:
                    continue          # not an address literal
                if 0x02004000 <= a < 0x020B0000:
                    continue          # an arm9 image symbol, not a poke
                if not mapped(a):
                    hit.append(lit)
            if hit:
                bad.append((t, sorted(set(hit))))
        print("SEED %s: %d function(s) touch an address ntr does not map"
              % (seed, len(bad)))
        for t, hit in bad:
            print("    %-34s %s" % (t, ",".join(hit)))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--root", default=".")
    ap.add_argument("--list", help="TSV of TUs (col 1 = stem, col 2 = src path)")
    ap.add_argument("--out")
    ap.add_argument("--seed", action="append", default=[])
    ap.add_argument("--ban", action="append", default=[])
    ap.add_argument("cmd", choices=["census", "slice", "hazard"])
    args = ap.parse_args()
    args.seed = [s for spec in args.seed for s in spec.split(",") if s]
    args.ban = [s for spec in args.ban for s in spec.split(",") if s]
    root = os.path.abspath(args.root)
    args.root = root
    g = Graph(root)
    rows = read_tu_list(args.list) if args.list else []
    if not rows and args.cmd != "hazard":
        sys.exit("--list is required for census and slice")
    {"census": cmd_census, "slice": cmd_slice, "hazard": cmd_hazard}[args.cmd](
        args, g, rows)


if __name__ == "__main__":
    main()
