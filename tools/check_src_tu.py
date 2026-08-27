"""Every reference a src_tu/ translation unit makes must still resolve.

WHY THIS EXISTS
---------------
PR #1643 renamed fourteen collision classes to the names the cartridge's own RTTI
carries and deleted the headers that held the old ones. It swept `src/` and
`include/`. It did not sweep `src_tu/`, and 26 of the tree's 41 merged translation
units stopped compiling: nine `#include`d a header that no longer existed, and the
rest called mangled symbols (`_ZN18MovingMeshColliderD1Ev`, `_ZN12WithMeshClsnC1Ev`,
...) that no `config/**/symbols.txt` names any more.

NOTHING WENT RED. That is the defect this file closes.

An unbuildable source file is not a failing source file -- it is an ABSENT one.
`rombuild.py` still reported 106/106 exact, because a file that will not compile
contributes no object, so it contributes no mismatch either; it simply drops out of
coverage (notes: unbuildable-files-invisible). `port_refcheck.py` still passed its
393 references, because it audits `port/`'s references into `src/` and has never
looked at `src_tu/`. The only thing in the tree that noticed was
`pytest tools/test_tubuild.py`, which compiles exactly one TU -- ov045/PoleLift --
and had been red since #1643 landed, alongside test_bytegate and test_srcpath.

So this gate answers the question those gates cannot: does every name a merged TU
writes down still exist?

CHECKS
------
1. includes - every `#include "..."` in a src_tu file resolves, either against
              include/ or against the including file's own directory. Angle-bracket
              includes are the compiler's own and are out of scope.
2. symbols  - every Itanium-mangled `_Z...` token that appears in CODE (comments and
              string literals are stripped first, so prose naming a symbol is not a
              reference) either
                (a) is a symbol in config/**/symbols.txt, or
                (b) is a compiler-emitted variant -- a D2 base-object destructor, or
                    a _ZTV / _ZTI / _ZTS -- of a class the ROM DOES name elsewhere.
              (b) is deliberately narrow: it forgives the vague-linkage symbols a C++
              TU emits for its own classes, and forgives nothing about a class the
              cartridge has no symbol for at all -- which is exactly the shape a
              retired class name leaves behind.

WHAT IT DOES NOT CHECK
----------------------
Types, signatures and layout. A TU can pass this gate and still not compile, because
a header can retype a field or change a virtual's return type without renaming
anything. Catching those needs mwccarm, and mwccarm is not on the GitHub runner: it
lives on the private build box that pr-validate.yml relays to. This check is
stdlib-only over the working tree -- no compiler, no ROM, about a second over 41
files -- which is why it can gate every PR the way header-offsets.yml and
python-names.yml do.

tools/check_src_tu_compiles.py is the other half: it runs the compiler, so it runs
where the compiler is (tools/hooks/pre-push, the build box) and not here.

This section used to name the Actor and CastleWater TUs, now promoted at
src/game/actors/Actor.cpp and src/game/actors/CastleWater.cpp, as being in that
state. They were not: both compile, and neither they nor any header
has changed since this file landed, so the claim was wrong when it was written. The
one translation unit that really did not compile -- ov029/SwitchActivatedPlank, whose
placeholder `struct dBgW_Kc { int d; };` collided with the real class #1643 gave it --
went unnamed, because nothing had run a compiler over src_tu to find out which was
which. That is the whole argument for the compile gate in one sentence.

AN EMPTY CHECK IS NOT A PASS
----------------------------
This tree is full of gates that hollow-pass: `check_references.py` prints "skipping"
and exits 0 when no eligibility report exists, and `check_header_offsets.py --changed`
exits 0 when its diff is empty. A gate that finds nothing to do and reports success is
indistinguishable from a gate that is working. So this one FAILS if it finds no
translation units, no includes, or no mangled references -- if the work list is empty,
something moved and the gate is no longer pointed at the tree.

Usage:
  python tools/check_src_tu.py                 # check every src_tu TU
  python tools/check_src_tu.py --json          # machine-readable report on stdout
  python tools/check_src_tu.py --json out.json # ... or to a file
  python tools/check_src_tu.py --root src_tu   # point it somewhere else

Exit code 0 when every reference resolves, 1 otherwise (including an empty work list).
"""
import argparse
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
INCLUDE = REPO / "include"

SOURCE_SUFFIXES = (".c", ".cpp")
INCLUDE_RE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*"([^"]+)"', re.M)
MANGLED_RE = re.compile(r"\b_Z[A-Za-z0-9_]+")
# The compiler-emitted variants a C++ TU legitimately mints for its own classes.
EMITTED_RE = re.compile(r"^(?:_ZT[VIS](?P<a>\d+)(?P<an>.+)$|_ZN(?P<b>\d+)(?P<bn>.+?)D2Ev$)")


def strip_comments_and_strings(text: str) -> str:
    """Blank out /*...*/, //..., "..." and '...'.

    A symbol named in a comment is prose, not a reference: src_tu comments quote
    symbols that nothing defines on purpose (ShutterBob.cpp explains a relocation
    into `_ZN10ClsnEnable6EnableEP9ClsnOwner`, "which nothing defines"), and the
    banner comments wrap long mangled names across two lines, which would make any
    token-level check see a truncation. Newlines are preserved so line numbers in
    the report still point at the real line."""
    out, i, n = [], 0, len(text)
    while i < n:
        c = text[i]
        nxt = text[i + 1] if i + 1 < n else ""
        if c == "/" and nxt == "*":
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            out.append("\n" * text.count("\n", i, j))
            i = j
        elif c == "/" and nxt == "/":
            j = text.find("\n", i)
            j = n if j < 0 else j
            i = j
        elif c in "\"'":
            j = i + 1
            while j < n and text[j] != c:
                j += 2 if text[j] == "\\" else 1
            out.append("\n" * text.count("\n", i, min(j + 1, n)))
            i = min(j + 1, n)
        else:
            out.append(c)
            i += 1
    return "".join(out)


def load_symbols():
    """Every symbol name the delinked ROM configuration knows, and every class name
    that appears as the leading `_ZN<len><Class>` component of one."""
    names, classes = set(), set()
    for path in sorted(REPO.glob("config/**/symbols.txt")):
        for line in path.read_text(errors="replace").splitlines():
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            sym = line.split()[0]
            names.add(sym)
            mo = re.match(r"_ZN[KV]*(\d+)(.+)", sym)
            if mo:
                ln = int(mo.group(1))
                if len(mo.group(2)) >= ln:
                    classes.add(mo.group(2)[:ln])
            mo = re.match(r"_ZT[VIS](\d+)(.+)", sym)
            if mo:
                ln = int(mo.group(1))
                if len(mo.group(2)) >= ln:
                    classes.add(mo.group(2)[:ln])
    return names, classes


def compiler_emitted_for(tok: str):
    """If `tok` is a vague-linkage symbol a TU mints itself, the class it belongs to."""
    mo = EMITTED_RE.match(tok)
    if not mo:
        return None
    ln = int(mo.group("a") or mo.group("b"))
    name = mo.group("an") or mo.group("bn")
    return name[:ln] if len(name) >= ln else None


def line_of(text: str, pos: int) -> int:
    return text.count("\n", 0, pos) + 1


def check(root: pathlib.Path):
    failures = []
    sources = sorted(p for p in root.rglob("*") if p.suffix in SOURCE_SUFFIXES)
    symbols, rom_classes = load_symbols()
    n_includes = n_refs = 0

    for src in sources:
        rel = src.relative_to(REPO).as_posix()
        raw = src.read_text(errors="replace")

        for mo in INCLUDE_RE.finditer(raw):
            target = mo.group(1)
            n_includes += 1
            if (INCLUDE / target).is_file() or (src.parent / target).is_file():
                continue
            failures.append({
                "check": "includes", "file": rel, "line": line_of(raw, mo.start()),
                "message": f'#include "{target}" names a header that does not exist '
                           f"(looked in include/ and {src.parent.relative_to(REPO).as_posix()}/)",
            })

        code = strip_comments_and_strings(raw)
        seen = {}
        for mo in MANGLED_RE.finditer(code):
            seen.setdefault(mo.group(0), line_of(code, mo.start()))
        for tok, line in sorted(seen.items()):
            n_refs += 1
            if tok in symbols:
                continue
            cls = compiler_emitted_for(tok)
            if cls is not None and cls in rom_classes:
                continue
            failures.append({
                "check": "symbols", "file": rel, "line": line,
                "message": f"{tok} is in no config/**/symbols.txt -- the name was "
                           f"retired, misspelled, or never existed",
            })

    # An empty check is not a pass; see the module docstring.
    if not sources:
        failures.append({"check": "worklist", "file": root.as_posix(), "line": 0,
                         "message": "no translation units found -- this gate is no "
                                    "longer pointed at any source"})
    if sources and not n_includes:
        failures.append({"check": "worklist", "file": root.as_posix(), "line": 0,
                         "message": f"{len(sources)} translation unit(s) but zero "
                                    f'#include "..." lines -- the include scan is broken'})
    if sources and not n_refs:
        failures.append({"check": "worklist", "file": root.as_posix(), "line": 0,
                         "message": f"{len(sources)} translation unit(s) but zero mangled "
                                    f"references -- the symbol scan is broken"})
    if not symbols:
        failures.append({"check": "worklist", "file": "config/**/symbols.txt", "line": 0,
                         "message": "no ROM symbols loaded -- every reference would "
                                    "'resolve' against an empty set"})

    return {
        "schemaVersion": 1,
        "ok": not failures,
        "checked": {"sources": len(sources), "includes": n_includes,
                    "references": n_refs, "romSymbols": len(symbols)},
        "failed": len(failures),
        "failures": failures,
    }


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--root", default="src_tu",
                    help="directory of translation units to check (default: src_tu)")
    ap.add_argument("--json", nargs="?", const="-", metavar="PATH",
                    help="write the JSON report to PATH, or stdout with no PATH")
    args = ap.parse_args(argv)

    root = REPO / args.root
    if not root.is_dir():
        print(f"check_src_tu: no such directory: {args.root}", file=sys.stderr)
        return 1

    report = check(root)

    if args.json:
        text = json.dumps(report, indent=2) + "\n"
        if args.json == "-":
            sys.stdout.write(text)
        else:
            # build/ is gitignored, so a fresh CI checkout has no such directory --
            # writing the report must not be what fails the gate.
            out = pathlib.Path(args.json)
            out.parent.mkdir(parents=True, exist_ok=True)
            out.write_text(text)

    c = report["checked"]
    print(f"check_src_tu: {c['sources']} translation unit(s), {c['includes']} include(s), "
          f"{c['references']} mangled reference(s), against {c['romSymbols']} ROM symbols")
    if report["ok"]:
        print("check_src_tu: every reference resolves.")
        return 0
    for f in report["failures"]:
        print(f"  {f['check']:8} {f['file']}:{f['line']}: {f['message']}")
    print(f"check_src_tu: {report['failed']} unresolved reference(s).")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
