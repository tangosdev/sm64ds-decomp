"""Build time RATCHET against NEW inferred vtable stub bodies getting seated.

Some src/ bodies were never decompiled from the ROM. They were guessed from a
vtable slot: the tool saw a class had a virtual in slot N, knew the address the
ROM parked there, and wrote a plausible C body for it. Every such body carries
the marker line

    recovered from vtable slot identity

in a header comment. There are 712 of these markers in src/ today. They are
behavioral GUESSES, not verified decompilations, so a body that looks right can
still be wrong in a way no byte gate on the ROM catches, because the ROM never
built from this source.

A guessed body only affects the port once it is SEATED: once a port vtable fill
in port/hal (actor_classes*.cpp and friends) puts that body's symbol into a live
class table. A seated guess runs. An unseated guess is dead source. 143 of the
712 are seated today.

This guard is the "do not let it happen again" mechanism. It does not try to fix
the seated guesses. It freezes the CURRENT seated set into a committed baseline
allowlist (inferred_stub_baseline.txt) and fails the build if the live seated
set ever grows past it: a new guessed body seated, or a brand new guessed file
seated. The baseline can only SHRINK. When a guess is fixed (its marker removed
because it became a faithful decomp) or de-seated (dropped from every vtable
fill), it falls out of the live set and the guard reports that the baseline can
be tightened. Tightening is never silent: it happens only under --update.

SEATED, precisely, is the conjunction of two conditions on a marker file:

  1. its TU is in the port build's active source set, enumerated exactly the way
     closestplayer_guard enumerates it: BOTH the slice gate active src/ lines in
     port/slice_gate*.txt AND the literal .c / .cpp paths CMakeLists.txt adds
     through list(APPEND ..._SOURCES), set(..._SOURCES ...), add_executable(...)
     or target_sources(...); and

  2. its @symbol is referenced, outside comments, in a port/hal file. A port/hal
     reference is what a vtable fill is: the class table naming the body's
     symbol. Comments are blanked before the reference scan for the same reason
     closestplayer_guard blanks them: a hal file often documents a symbol in
     prose (see actor_classes.cpp), and prose is not a seating.

A file that is inferred but unseated (marker present, but not in the active TU
set, or never named by a hal fill) is latent and does not count. That is the set
the port is meant to work DOWN: seat none of them anew, and retire the seated
ones as they get truly decompiled.

Wired ahead of cmake configure in build-port.cmd, next to closestplayer_guard.

LIMITATION: identical to closestplayer_guard. The active TU set is read
statically from port/slice_gate*.txt and the literal source paths in
port/CMakeLists.txt. A source pulled in through a path the reader cannot resolve
statically (a generated file, a glob, a computed variable) would not be seen.
Those are rare and are not how a body gets seated today.
"""

import os
import re
import sys

MARKER = "recovered from vtable slot identity"

# The mangled address symbol a marker file exports, one per file, on its
# `// @symbol NAME` line. Two shapes exist: func_ovNNN_ADDR for an overlay body
# and func_ADDR for an arm9 body.
SYMBOL_TOKEN = re.compile(r"func_ov[0-9a-fA-F]+_[0-9a-fA-F]+|func_[0-9a-fA-F]{8}")
SYMBOL_LINE = re.compile(r"^//\s*@symbol\s+(\S+)")

# CMake commands that ADD sources to the build. Identical to closestplayer_guard
# so the two guards agree on exactly which TUs the build compiles. Property
# commands like set_source_files_properties are intentionally excluded.
SOURCE_ADDING_CMD = re.compile(
    r"^\s*(?:"
    r"list\s*\(\s*APPEND\s+[A-Za-z0-9_]*SOURCES\b"
    r"|set\s*\(\s*[A-Za-z0-9_]*SOURCES\b"
    r"|add_executable\s*\("
    r"|target_sources\s*\("
    r")"
)

# A quoted literal path ending in .c or .cpp, capturing the inside of the quotes.
CMAKE_PATH = re.compile(r'"([^"]+?\.(?:c|cpp))"')


def port_root():
    """The port/ directory, resolved from this file, no absolute paths."""
    return os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def repo_root():
    """The repo root, one level above port/."""
    return os.path.dirname(port_root())


def baseline_path():
    """The committed allowlist, next to this script."""
    return os.path.join(os.path.dirname(os.path.abspath(__file__)),
                        "inferred_stub_baseline.txt")


def blank_comments(src):
    """Replace C and C++ comment bodies with spaces, preserving newlines.

    A hal fill or a marker file documents symbols in prose; that prose must not
    count as a reference or as a live symbol line. Newlines are kept so nothing
    downstream that cares about line numbers is thrown off.
    """
    out = []
    i = 0
    n = len(src)
    while i < n:
        two = src[i:i + 2]
        if two == "//":
            j = src.find("\n", i)
            if j == -1:
                j = n
            out.append(" " * (j - i))
            i = j
        elif two == "/*":
            j = src.find("*/", i + 2)
            if j == -1:
                j = n
            else:
                j += 2
            out.append("".join(c if c == "\n" else " " for c in src[i:j]))
            i = j
        else:
            out.append(src[i])
            i += 1
    return "".join(out)


def _add(ordered, seen, repo_dir, abs_path):
    """Register one resolved path if it exists and is new."""
    abs_path = os.path.normpath(abs_path)
    if abs_path in seen:
        return
    seen.add(abs_path)
    if not os.path.isfile(abs_path):
        return
    rel = os.path.relpath(abs_path, repo_dir).replace("\\", "/")
    ordered.append((rel, abs_path))


def gate_active_files(port_dir, repo_dir, ordered, seen):
    """Path (a): every src/ TU a slice gate activates.

    Mirrors the CMakeLists.txt gate foreach exactly: strip the line, skip blank
    lines and lines starting with '#', treat the rest as a repo relative path,
    keep the ones under src/ that exist.
    """
    gate_names = sorted(
        n for n in os.listdir(port_dir)
        if n.startswith("slice_gate") and n.endswith(".txt")
    )
    for name in gate_names:
        gate_path = os.path.join(port_dir, name)
        with open(gate_path, "r", encoding="utf-8", errors="replace") as fh:
            for raw in fh:
                line = raw.strip()
                if not line or line.startswith("#"):
                    continue
                rel = line.replace("\\", "/")
                if not rel.startswith("src/"):
                    continue
                _add(ordered, seen, repo_dir, os.path.join(repo_dir, rel))


def cmake_appended_files(port_dir, repo_dir, ordered, seen):
    """Path (b): every literal .c / .cpp path CMakeLists.txt adds as a source.

    Walks port/CMakeLists.txt tracking whether the current line sits inside a
    source adding command body, from the opening command line to its matching
    close paren by paren balance, and collects the literal quoted .c / .cpp
    paths, resolving the two source root variables the file uses. A path holding
    a gate loop variable or a generated file marker is skipped: gate loop paths
    are already covered by path (a), and generated files are not statically
    readable.
    """
    cml = os.path.join(port_dir, "CMakeLists.txt")
    depth = 0
    in_body = False
    with open(cml, "r", encoding="utf-8", errors="replace") as fh:
        for raw in fh:
            line = raw.split("#", 1)[0]
            if not in_body and SOURCE_ADDING_CMD.match(line):
                in_body = True
                depth = 0
            if in_body:
                for m in CMAKE_PATH.finditer(line):
                    literal = m.group(1)
                    if "${line}" in literal or "${CMAKE_BINARY_DIR}" in literal:
                        continue
                    resolved = literal
                    resolved = resolved.replace(
                        "${CMAKE_CURRENT_SOURCE_DIR}", port_dir)
                    resolved = resolved.replace(
                        "${PORT_REPO_ROOT_ABS}", repo_dir)
                    if "${" in resolved:
                        continue
                    _add(ordered, seen, repo_dir, resolved)
                depth += line.count("(") - line.count(")")
                if depth <= 0:
                    in_body = False


def build_tu_set(port_dir, repo_dir):
    """Union of path (a) gate active TUs and path (b) CMake appended TUs."""
    ordered = []
    seen = set()
    gate_active_files(port_dir, repo_dir, ordered, seen)
    cmake_appended_files(port_dir, repo_dir, ordered, seen)
    return ordered


def marker_symbol(abs_path):
    """The @symbol a marker file exports, or None. Reads the @symbol line
    directly; that line is authoritative and one per file."""
    with open(abs_path, "r", encoding="utf-8", errors="replace") as fh:
        for raw in fh:
            m = SYMBOL_LINE.match(raw)
            if m:
                return m.group(1).strip()
    return None


def hal_referenced_symbols(port_dir):
    """Every func_ symbol named outside comments in any port/hal file.

    A port/hal reference is a vtable fill seating. Comments are blanked so a
    symbol documented in prose does not read as a seating.
    """
    refs = set()
    hal_dir = os.path.join(port_dir, "hal")
    if not os.path.isdir(hal_dir):
        return refs
    for name in sorted(os.listdir(hal_dir)):
        if not (name.endswith(".cpp") or name.endswith(".h")
                or name.endswith(".c")):
            continue
        with open(os.path.join(hal_dir, name), "r",
                  encoding="utf-8", errors="replace") as fh:
            code = blank_comments(fh.read())
        refs.update(SYMBOL_TOKEN.findall(code))
    return refs


def live_seated_set(port_dir, repo_dir):
    """The current seated inferred stub symbols, sorted.

    A symbol is in the set when its marker file is in the active build TU set
    AND its symbol is referenced by a port/hal vtable fill.
    """
    hal_refs = hal_referenced_symbols(port_dir)
    seated = set()
    for rel, abs_path in build_tu_set(port_dir, repo_dir):
        if not rel.startswith("src/"):
            continue
        with open(abs_path, "r", encoding="utf-8", errors="replace") as fh:
            text = fh.read()
        if MARKER not in text:
            continue
        sym = marker_symbol(abs_path)
        if sym and sym in hal_refs:
            seated.add(sym)
    return sorted(seated)


def read_baseline():
    """The committed allowlist as a sorted list of symbols. Blank lines and
    lines starting with '#' are ignored."""
    path = baseline_path()
    if not os.path.isfile(path):
        return None
    syms = []
    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        for raw in fh:
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            syms.append(line)
    return sorted(set(syms))


BASELINE_HEADER = (
    "# inferred_stub_baseline.txt\n"
    "#\n"
    "# The frozen allowlist of SEATED inferred vtable stub bodies: src/ bodies\n"
    "# that carry the marker\n"
    "#\n"
    "#     recovered from vtable slot identity\n"
    "#\n"
    "# and whose @symbol is both in the port build's active TU set and named by a\n"
    "# port/hal vtable fill. These are behavioral guesses, not ROM verified\n"
    "# decompilations. This list is a RATCHET: it can only SHRINK.\n"
    "#\n"
    "# port/tools/inferred_stub_guard.py fails the build if the live seated set\n"
    "# holds any symbol not on this list, meaning a new guess got seated. When a\n"
    "# guessed body becomes a faithful decomp (its marker removed) or is dropped\n"
    "# from every vtable fill, it leaves the live set and this baseline may be\n"
    "# tightened with:  python port/tools/inferred_stub_guard.py --update\n"
    "# Never add a symbol here by hand to unblock a build.\n"
    "#\n"
    "# One symbol per line, sorted.\n"
)


def write_baseline(symbols):
    """Write the allowlist with its header, sorted, one symbol per line."""
    with open(baseline_path(), "w", encoding="utf-8", newline="\n") as fh:
        fh.write(BASELINE_HEADER)
        for sym in sorted(set(symbols)):
            fh.write(sym + "\n")


def main(argv):
    update = "--update" in argv[1:]
    port_dir = port_root()
    repo_dir = repo_root()

    live = live_seated_set(port_dir, repo_dir)
    baseline = read_baseline()

    if baseline is None:
        if update:
            write_baseline(live)
            print("inferred_stub_guard: wrote baseline with {} seated symbols."
                  .format(len(live)))
            return 0
        print("inferred_stub_guard FAIL: no baseline at {}. Create it with "
              "--update.".format(os.path.relpath(baseline_path(), repo_dir)
                                  .replace("\\", "/")))
        return 1

    live_set = set(live)
    base_set = set(baseline)
    new_seated = sorted(live_set - base_set)
    retired = sorted(base_set - live_set)

    if update:
        # Explicit tightening only. A new seat is still an error even here: the
        # baseline must never grow, so refuse to fold new guesses in.
        if new_seated:
            print("inferred_stub_guard FAIL: --update refuses to seat NEW "
                  "guessed bodies. The baseline can only shrink. Remedy the "
                  "seating first, then rerun --update. New seated:")
            for sym in new_seated:
                print("  {}".format(sym))
            return 1
        write_baseline(live)
        print("inferred_stub_guard: baseline tightened to {} seated symbols "
              "(was {}).".format(len(live), len(baseline)))
        if retired:
            print("Retired {} symbol(s):".format(len(retired)))
            for sym in retired:
                print("  {}".format(sym))
        return 0

    print("inferred_stub_guard: {} seated inferred stubs live, {} on baseline."
          .format(len(live_set), len(base_set)))

    if new_seated:
        print("")
        print("inferred_stub_guard FAIL: a NEW guessed vtable body got seated. "
              "These bodies are guesses (marker: recovered from vtable slot "
              "identity), not ROM decompilations, and must not enter a live "
              "vtable fill. The seated set can only shrink.")
        for sym in new_seated:
            print("  NEW SEAT: {}".format(sym))
        print("")
        print("REMEDY: do not seat a guessed body. Either de-seat it (remove "
              "its symbol from the port/hal vtable fill that names it) or "
              "replace the src body with a faithful decomp that drops the "
              "marker. Never add the symbol to "
              "port/tools/inferred_stub_baseline.txt to pass this gate.")
        return 1

    if retired:
        print("{} baseline symbol(s) are no longer seated. The baseline can be "
              "tightened: python port/tools/inferred_stub_guard.py --update"
              .format(len(retired)))
        for sym in retired:
            print("  retired: {}".format(sym))

    print("inferred_stub_guard OK: no new guessed vtable body seated.")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
