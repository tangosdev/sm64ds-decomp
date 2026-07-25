"""Near-miss crack harness: fast iteration loop for closing near-miss divergence.

Loads a function's stored draft and target bytes from nearmiss/db.jsonl and gives
you a 2-second oracle per hypothesis, so you can bisect what the compiler is doing
instead of guessing:

    import sys, pathlib
    sys.path.insert(0, str(pathlib.Path("tools")))
    from wallcrack import Target
    t = Target("func_ov006_020f4cd8")
    t.report(t.draft)      # reproduce the stored divergence count
    d, sz = t.div(src)     # oracle: (divergent words, size), reloc words wildcarded
    t.dump(src)            # side-by-side disasm of every mismatch,
                           # each line tagged regperm (register fields only) or SCHED

CLI:
    python tools/wallcrack.py <func_name>            # report + dump the stored draft
    python tools/wallcrack.py <func_name> --src f.c  # score a candidate file

Method notes for the coloring wall (pure register-permutation residue) live in
notes/mwccarm-codegen.md section 6aa. Short version: the ROM's coloring is
mwccarm's natural output; if your schedule matches and only the colors differ,
your source (often its #pragma opt_* crutch) is perturbing the allocator. Remove
the crutch, then reproduce its schedule effects with natural constructs
(volatile VRAM pointers, volatile globals plus an explicit temp, hoisted address
temps, the (base + i)[K] address form) and settle residual pair swaps with
declaration order last.

This oracle is for ITERATION only. The merge gates remain:
    python tools/match.py --c <file> --func <name> --addr <addr> --size <size> --trio
    python tools/linkcheck.py --name <name> --addr <addr> --size <size> --module <module>
Never put a non-matching file in src/.
"""
import io
import json
import os
import pathlib
import re
import subprocess
import sys
import tempfile

REPO = pathlib.Path(__file__).resolve().parents[1]
MW = REPO / "tools" / "mwccarm"
LICENSE = MW / "license.dat"
FLAGS = ("-O4,p -enum int -lang c99 -char signed -interworking -proc arm946e "
         "-gccext,on -msgstyle gcc")

from capstone import Cs, CS_ARCH_ARM, CS_MODE_ARM
from elftools.elf.elffile import ELFFile

md = Cs(CS_ARCH_ARM, CS_MODE_ARM)
_REG = re.compile(r"\b(r\d+|sb|sl|fp|ip|lr)\b")


def compile_c(src_text, lang_cpp=False, version="1.2/sp2p3"):
    """Compile source text. Returns (object_bytes, None) or (None, error)."""
    flags = FLAGS.replace("-lang c99", "-lang c++") if lang_cpp else FLAGS
    exe = MW / version / "mwccarm.exe"
    with tempfile.TemporaryDirectory() as td:
        cf = pathlib.Path(td) / ("t.cpp" if lang_cpp else "t.c")
        cf.write_text(src_text, encoding="utf-8", errors="replace")
        out_o = pathlib.Path(td) / "out.o"
        env = dict(os.environ, LM_LICENSE_FILE=str(LICENSE))
        r = subprocess.run([str(exe), *flags.split(), "-c", str(cf), "-o", str(out_o)],
                           capture_output=True, text=True, env=env, timeout=90)
        if r.returncode != 0 or not out_o.is_file():
            return None, r.stderr.strip()[:500]
        return out_o.read_bytes(), None


def extract(obj, func):
    """(code_bytes, reloc_word_offsets) for func, or (None, None)."""
    elf = ELFFile(io.BytesIO(obj))
    symtab = elf.get_section_by_name(".symtab")
    sym = next((s for s in symtab.iter_symbols() if s.name == func), None)
    if sym is None:
        return None, None
    sec = elf.get_section(sym["st_shndx"])
    code = sec.data()[sym["st_value"]:sym["st_value"] + sym["st_size"]]
    relocs = set()
    # match reloc section by sh_info, never by name (see tools/match.py)
    rel = next((s for s in elf.iter_sections()
                if s.header["sh_type"] in ("SHT_REL", "SHT_RELA")
                and s.header["sh_info"] == sym["st_shndx"]), None)
    if rel is not None:
        for r in rel.iter_relocations():
            o = r["r_offset"] - sym["st_value"]
            if 0 <= o < len(code):
                relocs.add(o & ~3)
    return code, relocs


def _skel(word, addr):
    ins = list(md.disasm(word, addr))
    if not ins:
        return None
    return ins[0].mnemonic + " " + _REG.sub("R", ins[0].op_str)


class Target:
    def __init__(self, name):
        self.name = name
        rec = None
        for line in open(REPO / "nearmiss" / "db.jsonl", encoding="utf-8"):
            d = json.loads(line)
            if d.get("name") == name and "target_hex" in d:
                rec = d  # last occurrence wins (later ingests supersede)
        if rec is None:
            raise KeyError(f"{name} not in nearmiss/db.jsonl (or entry has no target_hex)")
        self.rec = rec
        self.module = rec["module"]
        self.addr = rec["addr"] if isinstance(rec["addr"], str) else hex(rec["addr"])
        self.size = rec["size"]
        self.target = bytes.fromhex(rec["target_hex"])
        self.draft = rec["c_source"]
        self.lang_cpp = rec.get("lang") == "cpp" or self.draft.startswith("//cpp")

    def compile(self, src):
        cpp = self.lang_cpp or src.startswith("//cpp")
        obj, err = compile_c(src, lang_cpp=cpp)
        if obj is None:
            return None, None, err
        code, relocs = extract(obj, self.name)
        if code is None:
            return None, None, "function not found in object (check name/mangling)"
        return code, relocs, None

    def div(self, src):
        """(divergent_word_count, size). 999 marks a size drift; (None, err) a failure."""
        code, relocs, err = self.compile(src)
        if code is None:
            return None, err
        if len(code) != len(self.target):
            return 999, len(code)
        n = 0
        for o in range(0, len(code), 4):
            if o in relocs:
                continue
            if code[o:o + 4] != self.target[o:o + 4]:
                n += 1
        return n, len(code)

    def offsets(self, src):
        """Byte offsets of divergent words, or None on size drift/failure."""
        code, relocs, err = self.compile(src)
        if code is None or len(code) != len(self.target):
            return None
        return [o for o in range(0, len(code), 4)
                if o not in relocs and code[o:o + 4] != self.target[o:o + 4]]

    def dump(self, src, limit=60):
        """Side-by-side disasm of every mismatching word, tagged regperm or SCHED."""
        code, relocs, err = self.compile(src)
        if code is None:
            print("COMPILE FAIL:", err)
            return
        if len(code) != len(self.target):
            print(f"SIZE DRIFT: ours {len(code)} vs target {len(self.target)}")
        tl = {i.address: f"{i.mnemonic} {i.op_str}" for i in md.disasm(self.target, 0)}
        cl = {i.address: f"{i.mnemonic} {i.op_str}" for i in md.disasm(code, 0)}
        shown = 0
        for o in range(0, min(len(code), len(self.target)), 4):
            if o in relocs or code[o:o + 4] == self.target[o:o + 4]:
                continue
            regonly = _skel(self.target[o:o + 4], o) == _skel(code[o:o + 4], o)
            tag = "regperm " if regonly else "SCHED   "
            print(f"{tag}+{o:#06x} ROM: {tl.get(o, '?'):40s} OURS: {cl.get(o, '?')}")
            shown += 1
            if shown >= limit:
                print("... (truncated)")
                break

    def report(self, src):
        d, sz = self.div(src)
        print(f"{self.name}: div={d} size={sz} (target {len(self.target)})")
        return d, sz


def main():
    import argparse
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("name")
    ap.add_argument("--src", help="score this candidate file instead of the stored draft")
    ap.add_argument("--limit", type=int, default=60)
    args = ap.parse_args()
    t = Target(args.name)
    src = open(args.src, encoding="utf-8").read() if args.src else t.draft
    label = args.src or "stored draft"
    print(f"target {t.name} @ {t.addr} size {t.size} module {t.module} ({label})")
    t.report(src)
    t.dump(src, limit=args.limit)


if __name__ == "__main__":
    main()
