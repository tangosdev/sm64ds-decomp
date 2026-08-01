"""Import verified symbol names from an external symbols.x into our dsd config.

Source: SplattyDS/DynamicAllocationDecomp `symbols.x` (EU, same build as our ROM;
71.9% of its addresses land on dsd-found function starts). We import only the NAMES
(facts/interface), never their source code. Names are credited in CREDITS.md.

Conservative rules:
  - A symbol becomes a FUNCTION name only if its address is an exact function-start
    in our analysis, and a DATA name only if its address is an exact data-symbol start;
    otherwise it is recorded (in verified.tsv) but NOT forced into config -- inventing a
    symbol mid-blob would resplit the data segment.
  - We only rename slots still called `func_<addr>` / `data_<addr>` (never clobber names
    dsd already assigned, e.g. SDK signatures), and skip a name already used elsewhere in
    our config (no duplicate symbols).
  - DATA renames default to CONFIG-ONLY: a data symbol already referenced by name in a
    matched src file is DEFERRED (renaming it means rewriting those src files, which drags
    every referencing file -- including pre-existing NONMATCHING/blind ones -- into the PR
    and turns validate red even though the rename is byte-safe). Do the referenced ones in
    a quiet window with --with-referenced-data; that also rewrites the src references
    (relocs are address-based, so no bytes change and no match regresses).
  - C++/namespace names are sanitized to valid identifiers.

Writes:
  - config/**/symbols.txt   (function + data renames, in place)
  - src/**                  (data references, only with --with-referenced-data)
  - symbols/verified.tsv    (addr<TAB>name<TAB>kind  -- the artifact we own)

Usage:
  python tools/import_symbols.py                          # dry run
  python tools/import_symbols.py --apply                  # functions + config-only data
  python tools/import_symbols.py --apply --with-referenced-data   # also referenced data + src
"""
import argparse
import pathlib
import sys
import re

REPO = pathlib.Path(__file__).resolve().parent.parent
# Symbol sources, most-complete first (first name to claim an address wins).
SOURCES = [
    REPO / "reference" / "DynamicAllocationDecomp" / "symbols.x",
    REPO / "reference" / "SM64DS-ASM-Reference" / "symbols.x",
    REPO / "reference" / "MoreObjectsMod" / "ASM" / "symbols.x",
    REPO / "reference" / "MoreObjectsMod" / "ASM_HouseKeeper" / "symbols.x",
]
CONFIG = REPO / "config"
SRC = REPO / "src"
sys.path.insert(0, str(REPO / "tools"))
import srcpath as SP  # noqa: E402
OUT = REPO / "symbols" / "verified.tsv"
_DATA_REF_RE = re.compile(r"\b(data_(?:ov\d+_)?[0-9a-fA-F]{8})\b")


def sanitize(name: str) -> str:
    name = name.replace("::", "__")
    name = re.sub(r"[^A-Za-z0-9_]", "_", name)
    return name


def our_symbols():
    """addr -> (symbols.txt file, current name) for every function-start and data-start,
    plus the set of every symbol name already in our config (for collision checks)."""
    funcs, data, names = {}, {}, set()
    for sym in CONFIG.rglob("symbols.txt"):
        for line in sym.read_text(errors="ignore").splitlines():
            m = re.search(r"^(\S+)\s+kind:(function|data)\(.*?\)\s+addr:0x([0-9a-fA-F]+)", line)
            if not m:
                continue
            name, kind, addr = m.group(1), m.group(2), int(m.group(3), 16)
            names.add(name)
            (funcs if kind == "function" else data)[addr] = (sym, name)
    return funcs, data, names


def src_referenced_data():
    """The set of data_<addr> names currently referenced by name in src/."""
    refs = set()
    for s in SP.iter_sources():
        text = s.read_text(errors="ignore")
        if "data_" in text:
            refs.update(_DATA_REF_RE.findall(text))
    return refs


def parse_symbols_x():
    out = {}  # addr -> canonical name (first source/first line wins; aliases dropped)
    for src in SOURCES:
        if not src.is_file():
            continue
        nocomment = re.sub(r"/\*.*?\*/", "", src.read_text(errors="ignore"), flags=re.S)
        for m in re.finditer(r"([A-Za-z_][\w:]*)\s*=\s*0x0?([0-9a-fA-F]{6,8})\s*;", nocomment):
            out.setdefault(int(m.group(2), 16), sanitize(m.group(1)))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--apply", action="store_true", help="write renames (else dry run)")
    ap.add_argument("--with-referenced-data", action="store_true",
                    help="also rename data symbols already referenced in src/, rewriting those "
                         "src files. Disruptive - only in a quiet window. Default: config-only data.")
    args = ap.parse_args()

    funcs, dslots, names = our_symbols()
    theirs = parse_symbols_x()
    referenced = src_referenced_data()

    func_plan, data_plan = {}, {}   # symbols.txt file -> [(old, new)]
    src_renames = {}                # old data name -> new (rewritten in src, referenced case only)
    taken = set(names)
    n_func = n_data = n_deferred = n_collision = n_noslot = 0
    for addr, newname in theirs.items():
        if addr in funcs:
            _, cur = funcs[addr]
            if not cur.startswith("func_"):
                continue
            if newname in taken:
                n_collision += 1
                continue
            func_plan.setdefault(funcs[addr][0], []).append((cur, newname))
            taken.add(newname)
            n_func += 1
        elif addr in dslots:
            _, cur = dslots[addr]
            if not cur.startswith("data_"):
                continue
            if newname in taken:
                n_collision += 1
                continue
            if cur in referenced and not args.with_referenced_data:
                n_deferred += 1  # referenced by matched src -> needs the src-rewrite pass
                continue
            data_plan.setdefault(dslots[addr][0], []).append((cur, newname))
            if cur in referenced:
                src_renames[cur] = newname
            taken.add(newname)
            n_data += 1
        else:
            n_noslot += 1  # not a function/data start in our config -> recorded, not forced

    print(f"their active symbols: {len(theirs)}")
    print(f"renamable functions (func_* -> name):       {n_func}")
    print(f"renamable data, config-only (data_* -> name): {n_data}")
    print(f"data referenced in src, DEFERRED:            {n_deferred}"
          + ("" if args.with_referenced_data else "  (use --with-referenced-data)"))
    print(f"skipped, target name already in config:      {n_collision}")
    print(f"no config slot at that addr (record only):   {n_noslot}")

    OUT.parent.mkdir(exist_ok=True)
    with OUT.open("w", encoding="utf-8") as f:
        for addr in sorted(theirs):
            kind = "function" if addr in funcs else "data"
            f.write(f"0x{addr:08x}\t{theirs[addr]}\t{kind}\n")
    print(f"wrote {OUT.relative_to(REPO)} ({len(theirs)} symbols)")

    if not args.apply:
        print("\n(dry run) re-run with --apply to write renames.")
        return

    total = 0
    for sym_file, pairs in list(func_plan.items()) + list(data_plan.items()):
        text = sym_file.read_text(errors="ignore")
        for old, new in pairs:
            text = re.sub(rf"^{re.escape(old)}\b", new, text, count=1, flags=re.M)
            total += 1
        sym_file.write_text(text, encoding="utf-8")
    print(f"applied {total} symbol renames across config/")

    if src_renames:
        pat = re.compile(r"\b(" + "|".join(re.escape(k) for k in src_renames) + r")\b")
        touched = 0
        for src in SP.iter_sources():
            text = src.read_text(errors="ignore")
            if "data_" not in text:
                continue
            new = pat.sub(lambda m: src_renames[m.group(1)], text)
            if new != text:
                src.write_text(new, encoding="utf-8")
                touched += 1
        print(f"rewrote data-symbol references in {touched} src file(s)")


if __name__ == "__main__":
    main()
