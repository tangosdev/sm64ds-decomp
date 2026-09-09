#!/usr/bin/env python3
"""Census matched functions whose body spells a pool-load's destination address
as a bare integer literal instead of naming the data symbol.

THE BUG SHAPE. mwccarm compiles `return 0x20a94d4;` (or its decimal spelling,
`return 34247892;`) to the exact same bytes as `return data_020a94d4;`: both are
`ldr r0, [pc]; bx lr` plus a relocated pool word, so the byte gate cannot see the
difference and neither can --strict-relocs, because the reloc SLOT is a wildcard
either way -- what --strict-relocs checks is the destination the *compiled*
relocation points at, and a hardcoded literal has no relocation at all, so there
is nothing for it to catch here. func_02065b94 and func_02065b88 (SIGFIX3) were
found by hand this way: each returned a decimal integer that turned out to equal
the `to:` address of the function's own `kind:load` relocation in
config/<module>/relocs.txt. This sweeps the whole tree for the same coincidence.

WHAT THIS FINDS: a MATCHED function (asm_policy.counts_as_matched, i.e. no live
NONMATCHING banner) whose own kind:load relocation(s) -- the pool words its ROM
body actually loads, found by address range in config/<module>/relocs.txt -- have
a `to:` address that also appears as a bare integer literal (decimal or hex)
somewhere in the function's source, outside a comment or string. The literal can
be a `return N;` or a call argument; a load destination reaches C source through
either shape.

WHAT THIS DOES NOT DO. It does not decide the fix, and every row is a candidate,
not a verdict -- it has to be, because it does not check that the literal sits at
the SAME instruction as the reloc, only that both exist somewhere in the same
function. func_02058df4 (a multi-case switch returning different memory-region
boundaries) is the false positive that proved it necessary: case 0 returns
0x23c0000 as `mov r0, #0x23c0000` (a plain ARM immediate move -- 0x23c0000 fits
the rotated-8-bit encoding, so the compiler never touches the literal pool for
it), while case 4, unrelated, legitimately loads &data_023c0000 through a real
pool word elsewhere in the same function. Both spell the same address; only one
is backed by the relocation. Rewriting case 0 to `&data_023c0000` compiles to a
pool load and no longer matches the ROM's `mov` -- match.py catches it immediately,
which is the point: every row here MUST be byte-verified with match.py before it
is treated as a fix, never landed on the strength of the census alone. It also
cannot see a load whose destination has no name in symbols.txt: those rows are
reported with `sym=(none)` and need a symbol added before they can be named
honestly. Read the row, read the reloc, read the ROM.

    python tools/check_literal_load.py                # the census
    python tools/check_literal_load.py --summary       # plus scan totals
    python tools/check_literal_load.py --name func_x   # one function
    python tools/check_literal_load.py --json out.json
"""
import argparse
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import asm_policy               # noqa: E402
import relocs as R              # noqa: E402
import srcpath as SP            # noqa: E402
from check_call_arity import blank  # noqa: E402  (comment/string blanking, shared)

# `name kind:function(arm|thumb,size=0xNNN) addr:0xNNNNNNNN`
_FUNC_SYM_RE = re.compile(
    r"^(\S+)\s+kind:function\([a-z]+,size=0x([0-9a-fA-F]+)\)\s+addr:0x([0-9a-fA-F]+)")

# A bare integer literal: hex (0x...) or decimal, not glued to a surrounding
# identifier/dot (so it does not eat part of a hex constant or a float).
_LITERAL_RE = re.compile(r"(?<![\w.])(0[xX][0-9a-fA-F]+|[0-9]+)(?![\w.])")


def parse_functions(symbols_text):
    """[(name, addr, size)] for every function line in one symbols.txt's text.

    Aliases at the same address (mwccarm's `_dadd` sharing `func_01ff8000`'s
    address) carry size=0x0 and are kept -- they simply own no relocations of
    their own, so they never produce a load-in-range candidate."""
    out = []
    for line in symbols_text.splitlines():
        m = _FUNC_SYM_RE.match(line)
        if m:
            out.append((m.group(1), int(m.group(3), 16), int(m.group(2), 16)))
    return out


def load_relocs_in_range(relocs_by_from, addr, size):
    """[(from_addr, to_addr, to_module)] for every kind:load relocation inside
    [addr, addr+size). `to_module` is carried through (already normalized by
    relocs.load_relocs_file) because a load can cross modules -- func_02058df4
    (arm9) loads data_023c0000, which is defined in the dtcm symbols.txt, not
    arm9's; looking it up under the wrong module silently reports no symbol."""
    out = []
    for from_addr, (kind, to_addr, to_mod) in relocs_by_from.items():
        if kind == "load" and addr <= from_addr < addr + size:
            out.append((from_addr, to_addr, to_mod))
    return sorted(out)


def literal_hits(blanked_text, to_addr):
    """Every literal spelling of `to_addr` found in already-blanked source text.

    Returns the matched substrings (e.g. "34247892", "0x020a94d4") so a report
    can show exactly what was found, or [] if the address is not spelled out as
    a bare literal anywhere in the text."""
    hits = []
    for m in _LITERAL_RE.finditer(blanked_text):
        tok = m.group(1)
        try:
            val = int(tok, 16) if tok[:2] in ("0x", "0X") else int(tok, 10)
        except ValueError:
            continue
        if val == to_addr:
            hits.append(tok)
    return hits


def scan_module(module, functions, relocs_by_from, sym_index, text_getter):
    """Census rows for one module. `text_getter(name) -> str|None` reads a
    function's own source (None if no src/ file owns that symbol -- not yet
    decompiled, or living under a name this sweep cannot resolve)."""
    rows = []
    scanned = 0
    no_source = 0
    drafts = 0
    for name, addr, size in functions:
        candidates = load_relocs_in_range(relocs_by_from, addr, size)
        if not candidates:
            continue
        text = text_getter(name)
        if text is None:
            no_source += 1
            continue
        scanned += 1
        if not asm_policy.counts_as_matched(text):
            drafts += 1
            continue
        if asm_policy.is_asm_passthrough(text):
            # A hand-written asm block (Entry's CRT0, HAND-ASM PRIMITIVE) spells
            # every address as an `ldr r0, =0x...` operand by construction; that
            # is not the "C hides an honest symbol behind a literal" bug this
            # census hunts, and there is no C form to rewrite it into.
            continue
        blanked = blank(text)
        for from_addr, to_addr, to_mod in candidates:
            hits = literal_hits(blanked, to_addr)
            if not hits:
                continue
            sym = sym_index.get((R.normalize_module(to_mod), to_addr))
            rows.append({
                "name": name, "module": module, "addr": f"0x{addr:08x}",
                "from": f"0x{from_addr:08x}", "to": f"0x{to_addr:08x}",
                "to_module": R.normalize_module(to_mod),
                "sym": sym, "literal": hits[0],
            })
    return rows, scanned, no_source, drafts


def run(only_name=None):
    sym_index = R.load_all_syms()
    config_relocs = {}
    for module, path in R.iter_reloc_files(include_itcm_dtcm=True):
        config_relocs[module] = R.load_relocs_file(path)

    all_rows = []
    totals = {"scanned": 0, "no_source": 0, "drafts": 0}
    for module, sym_path in R.iter_symbol_files(include_itcm_dtcm=True):
        try:
            symbols_text = sym_path.read_text(encoding="utf-8", errors="ignore")
        except OSError:
            continue
        functions = parse_functions(symbols_text)
        if only_name:
            functions = [f for f in functions if f[0] == only_name]
            if not functions:
                continue
        relocs_by_from = config_relocs.get(module, {})

        def text_getter(name):
            p = SP.path_for(name)
            if p is None or not p.is_file():
                return None
            try:
                return p.read_text(encoding="utf-8", errors="ignore")
            except OSError:
                return None

        rows, scanned, no_source, drafts = scan_module(
            module, functions, relocs_by_from, sym_index, text_getter)
        all_rows.extend(rows)
        totals["scanned"] += scanned
        totals["no_source"] += no_source
        totals["drafts"] += drafts
    return all_rows, totals


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                  formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--json", help="write the census as JSON")
    ap.add_argument("--summary", action="store_true", help="also print scan totals")
    ap.add_argument("--name", help="restrict to one function symbol")
    args = ap.parse_args()

    rows, totals = run(only_name=args.name)

    print(f"=== matched functions whose body literal-spells a kind:load "
          f"destination ({len(rows)}) ===")
    print(f"{'name':<28}{'module':<8}{'from':<12}{'to':<12}{'literal':<14}sym")
    for r in rows:
        print(f"{r['name']:<28}{r['module']:<8}{r['from']:<12}{r['to']:<12}"
              f"{r['literal']:<14}{r['sym'] or '(none)'}")

    if args.summary:
        print()
        print("functions with a load in range and a src file  ", totals["scanned"])
        print("  no src/ file (not yet decompiled/named)       ", totals["no_source"])
        print("  NONMATCHING drafts (excluded)                 ", totals["drafts"])

    if args.json:
        pathlib.Path(args.json).write_text(json.dumps(rows, indent=2), encoding="utf-8")


if __name__ == "__main__":
    main()
