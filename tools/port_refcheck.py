"""Reference-integrity gate for the MSVC port at port/.

WHY THIS EXISTS
---------------
port/ builds a standalone MSVC host executable that references decomp src/
by literal path and symbol name:
  - port/slice_gate*.txt manifests list src/ (and a couple of port/unmatched)
    files the host build compiles, one repo-root-relative path per line.
  - port/CMakeLists.txt hardcodes GATE*_SYMS lists that hostgen.py resolves
    to src/<sym>.c or src/<sym>.cpp (mirroring the same probe here).
  - port/hal/*.cpp bridges MSVC linkage onto the decomp's func_XXXXXXXX /
    data_XXXXXXXX address-based naming through
    `#pragma comment(linker, "/alternatename:A=B")` and extern "C"
    declarations of those same names.

None of that is compiled or linked by the normal decomp toolchain (mwccarm,
the match/linkcheck pipeline), so a rename in src/ -- by hand, by
tools/cpp_rename.py, or a plain .c-to-.cpp / file-move -- can silently
strand a port/ reference. Nothing else catches it until someone attempts an
MSVC build.

This tool checks the port's references against the worktree's own files --
no compiler, no ROM -- so it runs in about a second and can gate every push.

CHECKS
------
1. manifests  - every non-comment, non-blank line in each of
                 port/slice_gate{1,2,3a,3b,4b,6,7,8,9}.txt names a path that
                 exists in the repo.
2. cmake syms - every symbol in port/CMakeLists.txt's hostgen `set(*_SYMS ...)`
                 lists resolves to src/<sym>.c or src/<sym>.cpp, the same
                 .c-then-.cpp probe the CMake itself runs at configure time.
3. hal links  - every func_XXXXXXXX / data_XXXXXXXX name that appears as the
                 alias or target of a `#pragma comment(linker,
                 "/alternatename:...")`, or as an extern "C" declaration, in
                 port/hal/*.cpp still appears (as an identifier) somewhere
                 under src/ or include/.

Scope note on check 3: only bare func_/data_ address-style names are
checked (the ones a src/ rename can actually break). Itanium-mangled names
declared extern "C" (e.g. _ZN6Memory16rootHeapIteratorE) and MSVC C++-mangled
symbols the pragmas reference (e.g. ?Crash@@YAXXZ) are decomp *identifiers*
that a rename tool would not touch the address-form of, so they are out of
scope here -- see AGENTS.md/MERGE.md for the human-facing rule.

Usage:
  python tools/port_refcheck.py
  python tools/port_refcheck.py --json                 # report on stdout
  python tools/port_refcheck.py --json build/port.json # report to a file

The JSON report is the stable contract behind tools/validate_merge.py's
--port-refcheck-report: schemaVersion, ok, checked, failed, a flat
`failures` list of {check, file, line, message}, and the same failures
grouped under `checks` for a human reading the file directly.
"""
import argparse
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
PORT = REPO / "port"

SLICE_GATE_SUFFIXES = ["1", "2", "3a", "3b", "4b", "6", "7", "8", "9"]
SRC_DIRS = [REPO / "src", REPO / "include"]
CODE_SUFFIXES = {".c", ".cpp", ".h", ".hpp"}

# func_XXXXXXXX / data_XXXXXXXX, with an optional ovNNN_ overlay qualifier
# (e.g. data_ov098_0213c380) -- the address-based naming a src/ rename can
# strand a port/ reference to.
IDENT_CORE = r"(?:func|data)_(?:ov\d+_)?[0-9a-fA-F]{8}"
IDENT_FULL_RE = re.compile(r"^" + IDENT_CORE + r"$")
IDENT_SCAN_RE = re.compile(r"\b" + IDENT_CORE + r"\b")

SET_SYMS_RE = re.compile(r"\bset\(\s*(\w+_SYMS)\b(.*?)\)", re.DOTALL)
PRAGMA_RE = re.compile(r'#pragma\s+comment\(\s*linker\s*,\s*"(/alternatename:[^"]*)"\s*\)')
EXTERN_C_RE = re.compile(r'extern\s*"C"')


class Failure:
    def __init__(self, file, line, message):
        self.file = file
        self.line = line
        self.message = message

    def __str__(self):
        return f"{self.file}:{self.line}: {self.message}"

    def to_dict(self):
        return {"file": self.file, "line": self.line, "message": self.message}


def _line_at(text, pos):
    return text.count("\n", 0, pos) + 1


def _mask_comments(text):
    """Blank out // and /* */ comment interiors, preserving length and
    newlines, so brace-matching and keyword search never trip over prose
    like "the extern \"C\" side of..." inside a comment."""
    out = []
    i, n = 0, len(text)
    while i < n:
        c = text[i]
        if c == "/" and i + 1 < n and text[i + 1] == "/":
            j = text.find("\n", i)
            j = n if j == -1 else j
            out.append(" " * (j - i))
            i = j
        elif c == "/" and i + 1 < n and text[i + 1] == "*":
            j = text.find("*/", i + 2)
            j = n if j == -1 else j + 2
            out.append("".join(ch if ch == "\n" else " " for ch in text[i:j]))
            i = j
        else:
            out.append(c)
            i += 1
    return "".join(out)


# ---- check 1: slice manifests -----------------------------------------------

def check_manifests():
    """Every path listed in a slice_gate*.txt manifest exists in the repo."""
    failures = []
    checked = 0
    for suffix in SLICE_GATE_SUFFIXES:
        manifest = PORT / f"slice_gate{suffix}.txt"
        rel = manifest.relative_to(REPO).as_posix()
        if not manifest.exists():
            failures.append(Failure(rel, 0, "manifest file not found"))
            continue
        text = manifest.read_text(encoding="utf-8", errors="ignore")
        for lineno, raw in enumerate(text.splitlines(), start=1):
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            checked += 1
            if not (REPO / line).exists():
                failures.append(Failure(rel, lineno, f"{line} not found (renamed?)"))
    return checked, failures


# ---- check 2: CMake hostgen symbol lists ------------------------------------

def check_cmake_symbols():
    """Every symbol in a `set(GATE*_SYMS ...)` list resolves to src/<sym>.c
    or src/<sym>.cpp, mirroring the EXISTS-then-.c-else-.cpp probe the CMake
    itself runs (port/CMakeLists.txt, one foreach per hostgen gate)."""
    cmake = PORT / "CMakeLists.txt"
    rel = cmake.relative_to(REPO).as_posix()
    if not cmake.exists():
        return 0, [Failure(rel, 0, "CMakeLists.txt not found")]

    text = cmake.read_text(encoding="utf-8", errors="ignore")
    masked = _mask_comments(text)
    failures = []
    checked = 0
    for m in SET_SYMS_RE.finditer(masked):
        list_name = m.group(1)
        body_start = m.start(2)
        for tok in re.finditer(r"\S+", m.group(2)):
            sym = tok.group(0)
            checked += 1
            lineno = _line_at(text, body_start + tok.start())
            if not ((REPO / "src" / f"{sym}.c").exists()
                    or (REPO / "src" / f"{sym}.cpp").exists()):
                failures.append(Failure(
                    rel, lineno,
                    f"symbol '{sym}' ({list_name}) has no src/{sym}.c or "
                    f"src/{sym}.cpp (renamed?)"))
    return checked, failures


# ---- check 3: port/hal linkage bridges --------------------------------------

def _strip_decoration(spelling):
    """Undo one layer of MSVC decoration on an /alternatename spelling and
    return the bare core name -- or None if it is not a bare identifier
    (still mangled, still suffixed, etc). Only a name that fully reduces to
    plain text is a candidate for the func_/data_ pattern check.

    ?data_0209b44c@@3CA          -> data_0209b44c   (C++-mangled variable)
    _data_020a0e68               -> data_020a0e68   (cdecl decoration)
    __ZN6Memory...E              -> ZN6Memory...E   (not func_/data_, skipped)
    _data_0209b44c_c             -> data_0209b44c_c (suffixed local storage,
                                                       correctly NOT matching
                                                       the bare pattern)
    """
    spelling = spelling.strip()
    if spelling.startswith("__imp_"):
        spelling = spelling[len("__imp_"):]
    if spelling.startswith("?"):
        at = spelling.find("@")
        return spelling[1:at] if at != -1 else spelling[1:]
    return spelling.lstrip("_")


def _extern_c_spans(masked_text):
    """(start, end) character spans covering each `extern "C"` occurrence in
    `masked_text` through the end of its scope: the whole `{ ... }` block for
    the block form, or the single declaration (plus its function body, if
    any) for `extern "C" <decl>;` / `extern "C" <decl> { ... }`."""
    spans = []
    n = len(masked_text)
    for m in EXTERN_C_RE.finditer(masked_text):
        j = m.end()
        while j < n and masked_text[j] in " \t\r\n":
            j += 1
        if j < n and masked_text[j] == "{":
            depth, k = 0, j
            while k < n:
                if masked_text[k] == "{":
                    depth += 1
                elif masked_text[k] == "}":
                    depth -= 1
                    if depth == 0:
                        k += 1
                        break
                k += 1
            spans.append((m.start(), k))
        else:
            k = j
            while k < n and masked_text[k] not in ";{":
                k += 1
            if k < n and masked_text[k] == "{":
                depth = 0
                while k < n:
                    if masked_text[k] == "{":
                        depth += 1
                    elif masked_text[k] == "}":
                        depth -= 1
                        if depth == 0:
                            k += 1
                            break
                    k += 1
            elif k < n and masked_text[k] == ";":
                k += 1
            spans.append((m.start(), k))
    return spans


def _scan_symbol_index():
    """Pure-Python fallback: every func_/data_ token under src/ or include/,
    built by reading every file. Correct but slow (~1s+ on this corpus) --
    only used if git is unavailable."""
    index = set()
    for root in SRC_DIRS:
        if not root.is_dir():
            continue
        for p in root.rglob("*"):
            if p.is_file() and p.suffix.lower() in CODE_SUFFIXES:
                try:
                    text = p.read_text(encoding="utf-8", errors="ignore")
                except OSError:
                    continue
                index.update(IDENT_SCAN_RE.findall(text))
    return index


def _present(names):
    """Which of `names` (func_/data_ identifiers) appear as whole-word text
    somewhere under src/ or include/. A single `git grep -F -w` pass over the
    whole tree is ~10x faster than reading and regex-scanning ~11k files in
    Python (git grep: ~0.15s here vs ~1.2s), so it is tried first; a plain
    scan is the fallback for a non-git checkout or if git is missing."""
    names = sorted(set(names))
    if not names:
        return set()
    import subprocess
    args = ["git", "grep", "--no-color", "-F", "-w", "-h", "-o"]
    for n in names:
        args += ["-e", n]
    args += ["--"] + [d.name for d in SRC_DIRS if d.is_dir()]
    try:
        proc = subprocess.run(args, cwd=REPO, capture_output=True, text=True)
    except (OSError, subprocess.SubprocessError):
        proc = None
    if proc is not None and proc.returncode in (0, 1):
        # 0 = matches found, 1 = git grep ran fine but found nothing
        found = {line.strip() for line in proc.stdout.splitlines() if line.strip()}
        return found & set(names)
    return _scan_symbol_index() & set(names)


def check_hal_links():
    """#pragma alternatename aliases/targets and extern "C" declarations of
    func_/data_ names in port/hal/*.cpp must still name something that
    exists in src/ or include/."""
    hal_dir = PORT / "hal"
    if not hal_dir.is_dir():
        return 0, [Failure("port/hal", 0, "hal directory not found")]

    # Pass 1: collect every candidate reference without checking existence
    # yet, so all the (few hundred) names can be looked up in one batched
    # `git grep` pass instead of one lookup each.
    refs = []  # (rel, lineno, name, message)
    for path in sorted(hal_dir.glob("*.cpp")):
        rel = path.relative_to(REPO).as_posix()
        text = path.read_text(encoding="utf-8", errors="ignore")
        masked = _mask_comments(text)

        for m in PRAGMA_RE.finditer(masked):
            lineno = _line_at(text, m.start())
            body = m.group(1)[len("/alternatename:"):]
            alias, _, target = body.partition("=")
            for spelling in (alias, target):
                core = _strip_decoration(spelling)
                if not core or not IDENT_FULL_RE.match(core):
                    continue
                refs.append((rel, lineno, core,
                              f"alternatename references '{core}' (from "
                              f"'{spelling.strip()}'), not found in src/ or "
                              f"include/ (renamed?)"))

        for start, end in _extern_c_spans(masked):
            for m in IDENT_SCAN_RE.finditer(masked, start, end):
                name = m.group(0)
                lineno = _line_at(text, m.start())
                refs.append((rel, lineno, name,
                             f'extern "C" declares \'{name}\', not found in '
                             f"src/ or include/ (renamed?)"))

    # Pass 2: one batched presence check, then report.
    present = _present(name for _, _, name, _ in refs)
    failures = []
    seen = set()  # (file, name) already reported, so a name isn't repeated
    for rel, lineno, name, message in refs:
        if name in present:
            continue
        key = (rel, name)
        if key in seen:
            continue
        seen.add(key)
        failures.append(Failure(rel, lineno, message))

    return len(refs), failures


CHECKS = [
    ("manifests", check_manifests),
    ("cmake-symbols", check_cmake_symbols),
    ("hal-links", check_hal_links),
]


def build_report():
    """Run every check and return the report JSON consumers see.

    `failures` is a flat, check-tagged copy of everything under `checks`, so a
    consumer that only wants "did it pass, and what broke" -- tools/validate_merge.py
    -- never has to walk the per-check grouping.
    """
    results = {}
    tagged = []
    total_checked = 0
    for name, fn in CHECKS:
        checked, failures = fn()
        results[name] = {"checked": checked, "failures": [f.to_dict() for f in failures]}
        total_checked += checked
        tagged.extend((name, f) for f in failures)
    return {
        "schemaVersion": 1,
        "ok": not tagged,
        "checked": total_checked,
        "failed": len(tagged),
        "failures": [dict(f.to_dict(), check=name) for name, f in tagged],
        "checks": results,
    }


def main():
    ap = argparse.ArgumentParser(
        description="Check port/'s literal src/ path and symbol references "
                     "for staleness left behind by renames.",
        formatter_class=argparse.RawDescriptionHelpFormatter, epilog=__doc__)
    ap.add_argument("--json", nargs="?", const="-", metavar="PATH",
                    help="write the JSON report to PATH (stdout if PATH is "
                         "omitted or '-')")
    args = ap.parse_args()

    report = build_report()
    all_failures = report["failures"]
    total_checked = report["checked"]

    if args.json == "-":
        print(json.dumps(report, indent=2))
        return 1 if all_failures else 0
    if args.json:
        # A report file leaves stdout free for the human summary below, which is
        # all a CI log of this phase would otherwise show.
        pathlib.Path(args.json).write_text(json.dumps(report, indent=2) + "\n",
                                           encoding="utf-8", newline="\n")

    for name, fn in CHECKS:
        r = report["checks"][name]
        icon = "OK  " if not r["failures"] else "FAIL"
        print(f"  [{icon}] {name:<14} {r['checked']} reference(s) checked, "
              f"{len(r['failures'])} stale")

    if all_failures:
        print("\nport-refcheck: STALE port/ references found:", file=sys.stderr)
        for f in all_failures:
            print(f"  {f['file']}:{f['line']}: {f['message']}", file=sys.stderr)
        print(f"\nport-refcheck: {total_checked} checked - {len(all_failures)} stale",
              file=sys.stderr)
        print("A src/ rename, .c-to-.cpp migration, or file move left port/ "
              "pointing at a name that no longer exists. Update the "
              "manifest / CMakeLists.txt / port/hal/*.cpp reference (see "
              "AGENTS.md), then re-run.", file=sys.stderr)
        return 1

    print(f"port-refcheck: {total_checked} checked - all references resolve")
    return 0


if __name__ == "__main__":
    sys.exit(main())
