"""Recover each class's ORIGINAL name from the cartridge's own RTTI, and say where
the decomp disagrees with it.

WHY THIS EXISTS
---------------
A class's `_ZTS` record is not a hint about the name, it IS the name: the Itanium
ABI stores the length-prefixed identifier verbatim, so `daObjCannonShutter_c`
occupies 24 bytes that a class spelled `CannonHatch` can only ever fill with 14.
That makes the ROM the authority and the decomp name the guess, and it means a
coined name is a permanent ceiling -- no amount of matching effort will let the
compiler emit an RTTI block for a class whose name it does not have (notes:
coined-names-block-data-verification).

Today that costs nothing, because vtables and typeinfo are delinked ROM blobs the
build copies verbatim and no source asks the compiler to generate them. It becomes
load-bearing the moment a class is migrated far enough that mwccarm emits its own
RTTI. This tool exists so that decision is made against measured evidence rather
than against whichever name happens to be in symbols.txt.

`notes/model-rtti-names.md` did this by hand for the eleven model/animation classes
and is the regression oracle for this tool -- see --self-test.

HOW A RECORD IS READ
--------------------
    vtable V     ->  typeinfo pointer at V-4
    typeinfo T   ->  [0] type_info vtable  (which KIND of record this is)
                     [4] name pointer      -> the `_ZTS` string, length-prefixed
                     [8] base / base count (kind-dependent, not needed here)

Pointers are followed. A name string is NEVER read because it happens to sit next
to a vtable: the name pool is physically interleaved with the vtables, so the
string after `_ZTV15MaterialChanger` belongs to a different class entirely. That
adjacency trap is what notes/model-rtti-names.md warns about, and reading through
the typeinfo pointer is what avoids it.

WHAT IT DOES NOT DO
-------------------
It does not rename anything and it does not edit any config. A class rename has to
rewrite mangled symbols (the LENGTH PREFIX changes: `_ZN11CannonHatch8BehaviorEv`
becomes `_ZN20daObjCannonShutter_c8BehaviorEv`), move source FILENAMES, and update
delinks.txt -- see tools/class_rename.py. This tool only produces the evidence.

Usage:
  python tools/rom_class_names.py                    # table of disagreements
  python tools/rom_class_names.py --all              # every class, agreeing or not
  python tools/rom_class_names.py --json out.json
  python tools/rom_class_names.py --module ov002
  python tools/rom_class_names.py --self-test        # check against the hand-built oracle
"""
from __future__ import annotations

import argparse
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent

SYM_RE = re.compile(r"^(\S+)\s+kind:(\S+)\s+addr:0x([0-9a-fA-F]+)")
# `-  id: N` then `base_address: D` inside extracted/dsd/arm9_overlays/*.yaml.
OV_RE = re.compile(r"-\s+id:\s*(\d+)\s*\n\s+base_address:\s*(\d+)")
BASE_RE = re.compile(r"^base_address:\s*(\d+)", re.M)

# The oracle: notes/model-rtti-names.md, recovered by hand before this tool existed.
ORACLE = {
    0x0208e90c: "dExtSimpleModel_c",
    0x0208e868: "dExtShadowModel_c",
    0x0208e8a4: "dExtCommonModel_c",
    0x0208e7e4: "dExtFrameCtrl_c",
    0x0208e7c4: "dExtAnmTexSRT_c",
    0x0208e7d4: "dExtAnmTexPat_c",
    0x0208e7f4: "dExtAnmMaterial_c",
    0x0208e980: "dExtAnmModel_c",
    0x0208e9b4: "dExtAnm2Model_c",
    0x0208e94c: "dExtBlendAnmModel_c",
}


class Image:
    """One module's bytes plus the address they load at."""

    def __init__(self, name, base, data):
        self.name, self.base, self.data = name, base, data

    def word(self, addr):
        off = addr - self.base
        if off < 0 or off + 4 > len(self.data):
            return None
        return int.from_bytes(self.data[off:off + 4], "little")

    def cstr(self, addr):
        off = addr - self.base
        if off < 0 or off >= len(self.data):
            return None
        end = self.data.find(b"\0", off)
        if end < 0:
            return None
        try:
            return self.data[off:end].decode("ascii")
        except UnicodeDecodeError:
            return None


def _read_base(path):
    m = BASE_RE.search(path.read_text(encoding="utf-8"))
    return int(m.group(1)) if m else None


def modules():
    """(name, symbols.txt, Image) for arm9 and every overlay that has both halves."""
    out = []

    arm9_yaml = REPO / "extracted/dsd/arm9/arm9.yaml"
    arm9_bin = REPO / "extracted/dsd/arm9/arm9.bin"
    arm9_sym = REPO / "config/arm9/symbols.txt"
    if arm9_yaml.is_file() and arm9_bin.is_file() and arm9_sym.is_file():
        out.append(("arm9", arm9_sym,
                    Image("arm9", _read_base(arm9_yaml), arm9_bin.read_bytes())))

    bases = {}
    ovdir = REPO / "extracted/dsd/arm9_overlays"
    if ovdir.is_dir():
        for f in sorted(ovdir.glob("*.yaml")):
            for m in OV_RE.finditer(f.read_text(encoding="utf-8")):
                bases[int(m.group(1))] = int(m.group(2))
    for ovid, base in sorted(bases.items()):
        sym = REPO / f"config/arm9/overlays/ov{ovid:03d}/symbols.txt"
        binp = REPO / f"extracted/overlays/overlay_{ovid:04d}.bin"
        if sym.is_file() and binp.is_file():
            out.append((f"ov{ovid:03d}", sym,
                        Image(f"ov{ovid:03d}", base, binp.read_bytes())))
    return out


def parse_symbols(path):
    syms = []
    for line in path.read_text(encoding="utf-8", errors="replace").splitlines():
        m = SYM_RE.match(line.strip())
        if m:
            syms.append((m.group(1), int(m.group(3), 16)))
    return syms


def collect(only_module=None):
    """One row per vtable whose RTTI name can be read by following pointers."""
    rows = []
    for name, sym_path, img in modules():
        if only_module and name != only_module:
            continue
        if img.base is None:
            continue
        syms = parse_symbols(sym_path)
        # Every name recorded at each address, so an alias carrying the ROM name
        # can be recognised rather than reported as a disagreement.
        at = {}
        for s, a in syms:
            at.setdefault(a, []).append(s)
        zti = {a for a, names in at.items() if any(n.startswith("_ZTI") for n in names)}

        for s, addr in syms:
            if not s.startswith("_ZTV"):
                continue
            ti = img.word(addr - 4)
            if ti is None or ti not in zti:
                continue          # RTTI off for this class, or the slot is not a pointer
            zts_addr = img.word(ti + 4)
            if zts_addr is None:
                continue
            rom = img.cstr(zts_addr)
            if not rom or not rom[:1].isdigit():
                continue          # not a length-prefixed Itanium identifier
            decomp = s[4:]        # `_ZTV11CannonHatch` -> `11CannonHatch`
            aliases = sorted({n[4:] for n in at[addr] if n.startswith("_ZTV")})
            rows.append({
                "module": name,
                "vtable": f"0x{addr:08x}",
                "typeinfo": f"0x{ti:08x}",
                "decomp_mangled": decomp,
                "rom_mangled": rom,
                "decomp": _strip_len(decomp),
                "rom": _strip_len(rom),
                "agrees": decomp == rom,
                "aliased": rom in aliases,
                "aliases": aliases,
            })
    return rows


def _strip_len(mangled):
    m = re.match(r"^(\d+)(.*)$", mangled)
    if not m:
        return mangled
    n, rest = int(m.group(1)), m.group(2)
    return rest[:n] if len(rest) >= n else rest


def self_test():
    """Check the tool against notes/model-rtti-names.md, built by hand."""
    got = {int(r["vtable"], 16): r["rom"] for r in collect("arm9")}
    bad = []
    for addr, want in sorted(ORACLE.items()):
        have = got.get(addr)
        if have != want:
            bad.append(f"  0x{addr:08x}: expected {want!r}, tool says {have!r}")
    if bad:
        print(f"rom_class_names --self-test: {len(bad)}/{len(ORACLE)} MISMATCH")
        print("\n".join(bad))
        return 1
    print(f"rom_class_names --self-test: {len(ORACLE)}/{len(ORACLE)} rows match "
          f"notes/model-rtti-names.md")
    return 0


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0],
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--module", default=None, help="only this module, e.g. ov002")
    ap.add_argument("--all", action="store_true", help="include classes that agree")
    ap.add_argument("--json", nargs="?", const="-", metavar="PATH")
    ap.add_argument("--self-test", action="store_true")
    args = ap.parse_args(argv)

    if args.self_test:
        return self_test()

    rows = collect(args.module)
    if not rows:
        print("rom_class_names: no RTTI records found -- is extracted/ populated?",
              file=sys.stderr)
        return 1

    shown = rows if args.all else [r for r in rows if not r["agrees"]]

    if args.json:
        text = json.dumps({"schemaVersion": 1, "rows": shown}, indent=2) + "\n"
        if args.json == "-":
            sys.stdout.write(text)
        else:
            out = pathlib.Path(args.json)
            out.parent.mkdir(parents=True, exist_ok=True)
            out.write_text(text, encoding="utf-8", newline="\n")
    else:
        for r in shown:
            flag = "alias" if r["aliased"] else ("ok" if r["agrees"] else "NAKED")
            print(f"  {r['module']:6} {r['vtable']}  {flag:5} "
                  f"{r['decomp']:32} -> {r['rom']}")

    agree = sum(1 for r in rows if r["agrees"])
    aliased = sum(1 for r in rows if not r["agrees"] and r["aliased"])
    naked = sum(1 for r in rows if not r["agrees"] and not r["aliased"])
    print(f"\nrom_class_names: {len(rows)} class(es) with readable RTTI -- "
          f"{agree} agree, {aliased} disagree but an alias carries the ROM name, "
          f"{naked} carry no ROM name anywhere")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
