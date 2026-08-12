"""Build time guard against receiver dropping thiscall readers in the port.

Some src/ readers were decompiled as free functions that call an Actor
thiscall with empty parentheses, for example

    char* p = _ZN5Actor13ClosestPlayerEv();

Actor::ClosestPlayer is a thiscall: it reads `this` from the receiver
register. A zero argument call leaves that register holding whatever was
there, so on the host it dereferences a null base (null this + 0x5c) and
faults. The live readers were host copied to pass their own receiver (see
port/unmatched/Actor_ClosestPlayer_OverlayReaders.cpp and
Actor_ClosestPlayerWrappers.cpp). More raw readers still sit in src/ but
their overlays are not mounted yet, so they are latent. A source comment
cannot warn the future host author, because there is no arrival line for a
file that no build compiles.

This guard turns that hope into a build failure. It enumerates every TU the
port build actually compiles, by BOTH paths CMakeLists.txt feeds sources
through, and scans each for a zero argument call to a known receiver dropping
symbol. If a TU carrying such a call ever enters the build without being host
copied first, the build stops here with the fix spelled out.

The build compiles a TU one of two ways, and this guard covers both:

  (a) slice gate active: an uncommented src/ line in a port/slice_gate*.txt
      file. The gate foreach in CMakeLists.txt turns each such line into a
      SLICE<N>_SOURCES entry.

  (b) direct CMake source append: a literal .c or .cpp path fed straight to a
      source list in port/CMakeLists.txt, through list(APPEND ..._SOURCES),
      set(..._SOURCES ...), add_executable(...), or target_sources(...). This
      is how a host copy under port/unmatched normally enters a gate binary
      (Player_HeadBonk.cpp, the OnAttacked2 / OnKicked dispatchers, the TexSeq
      callers, RabbitKey_StateSeat.cpp, and more), and it is also how a src/
      TU can be added outside a gate. Most hosting happens on this path, so it
      is not an edge case.

Property only references (set_source_files_properties) are deliberately NOT
followed: they set a language or a flag on a path, they do not add it to the
build, so scanning them would fire on a raw src reader that is host copied and
correctly kept out of the build.

It is a build time check, wired ahead of cmake configure in build-port.cmd.

LIMITATION: the guard reads port/slice_gate*.txt and the literal source paths
in port/CMakeLists.txt. A source pulled in through a path the guard cannot
read statically (a generated file written at configure time, a glob, or a
variable whose value is computed) would not be seen. Those are rare and are
not how a hand written host copy or a raw reader enters the build today.
"""

import os
import re
import sys

# One row per receiver dropping symbol. Add a row here when a new thiscall
# reader that drops its receiver is found. Each row is:
#   symbol       the mangled name, for the human reading the report
#   pattern      a compiled regex matching the ZERO ARGUMENT call form. It
#                must match `SYMBOL()` (optional whitespace inside the
#                parens) and must NOT match the `(void)` declaration or any
#                call that passes an argument.
#   remedy       the fix text printed when this row finds an offender.
RULES = [
    {
        "symbol": "_ZN5Actor13ClosestPlayerEv",
        "pattern": re.compile(r"_ZN5Actor13ClosestPlayerEv\s*\(\s*\)"),
        "remedy": (
            "Actor::ClosestPlayer is thiscall. A zero argument call drops the "
            "receiver and faults on a null base (null this + 0x5c). Host copy "
            "this TU to pass its own receiver (see "
            "port/unmatched/Actor_ClosestPlayer_OverlayReaders.cpp) and comment "
            "the raw src out of its slice gate. Per seat gate item 4: a newly "
            "hosted overlay carrying a known raw reader."
        ),
    },
]

# CMake commands that ADD sources to the build. A literal .c / .cpp path
# inside one of these command bodies is compiled. Property commands like
# set_source_files_properties are intentionally excluded (see module docstring).
SOURCE_ADDING_CMD = re.compile(
    r"^\s*(?:"
    r"list\s*\(\s*APPEND\s+[A-Za-z0-9_]*SOURCES\b"      # list(APPEND FOO_SOURCES
    r"|set\s*\(\s*[A-Za-z0-9_]*SOURCES\b"                # set(FOO_SOURCES
    r"|add_executable\s*\("                              # add_executable(
    r"|target_sources\s*\("                             # target_sources(
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


def _add(ordered, seen, repo_dir, abs_path):
    """Register one resolved path if it exists and is new. Returns display rel."""
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

    Mirrors the CMakeLists.txt gate foreach exactly: strip the line, skip
    blank lines and lines that start with '#', treat the rest as a path
    relative to the repo root. Keep only the ones under src/ that exist.
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

    Walks port/CMakeLists.txt tracking whether the current line is inside a
    source adding command body (from the opening command line until its
    matching close paren, by paren balance). Inside such a body it collects
    literal quoted .c / .cpp paths, resolving the two variables the file uses
    for source roots:
        ${CMAKE_CURRENT_SOURCE_DIR}   ->  port/
        ${PORT_REPO_ROOT_ABS}         ->  repo root
    and the ../ that reaches the repo root from port/. A path holding ${line}
    or ${CMAKE_BINARY_DIR} (a gate loop variable or a generated file) is
    skipped: gate loop paths are already covered by path (a), and generated
    files are not statically readable.
    """
    cml = os.path.join(port_dir, "CMakeLists.txt")
    depth = 0            # open paren balance while inside a source adding body
    in_body = False
    with open(cml, "r", encoding="utf-8", errors="replace") as fh:
        for raw in fh:
            line = raw.split("#", 1)[0]  # drop CMake line comments
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
                        continue  # an unresolved variable, cannot read it
                    _add(ordered, seen, repo_dir, resolved)
                depth += line.count("(") - line.count(")")
                if depth <= 0:
                    in_body = False
    return


def build_tu_set(port_dir, repo_dir):
    """Union of path (a) gate active TUs and path (b) CMake appended TUs."""
    ordered = []
    seen = set()
    gate_active_files(port_dir, repo_dir, ordered, seen)
    cmake_appended_files(port_dir, repo_dir, ordered, seen)
    return ordered


def blank_comments(src):
    """Replace C and C++ comment bodies with spaces, preserving newlines.

    A host copy often documents the very bug it fixes by quoting the raw src
    spelling, for example the string `_ZN5Actor13ClosestPlayerEv()` inside a
    block comment (see port/unmatched/Ov085_Rabbit_b8dc.cpp). That is prose,
    not a call, so comments must not count. Line numbers are preserved so the
    reported line still points at the real source line. String literals are
    not stripped; the mangled symbol does not appear inside one in this tree,
    and keeping them avoids a fragile string parser.
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
            # Keep newlines inside the comment so line numbers stay aligned.
            out.append("".join(c if c == "\n" else " " for c in src[i:j]))
            i = j
        else:
            out.append(src[i])
            i += 1
    return "".join(out)


def scan_file(rel, abs_path):
    """Return a list of (rel, lineno, symbol, remedy) offenders in one TU.

    Comments are blanked first so a documented raw spelling in prose does not
    count as a live call.
    """
    offenders = []
    with open(abs_path, "r", encoding="utf-8", errors="replace") as fh:
        code = blank_comments(fh.read())
    for lineno, text in enumerate(code.splitlines(), start=1):
        for rule in RULES:
            if rule["pattern"].search(text):
                offenders.append((rel, lineno, rule["symbol"], rule["remedy"]))
    return offenders


def main():
    port_dir = port_root()
    repo_dir = repo_root()
    files = build_tu_set(port_dir, repo_dir)

    offenders = []
    for rel, abs_path in files:
        offenders.extend(scan_file(rel, abs_path))

    if not offenders:
        print(
            "closestplayer_guard OK: no zero argument receiver dropping calls "
            "in {} build active TUs (slice gate plus direct CMake append)."
            .format(len(files))
        )
        return 0

    for rel, lineno, symbol, _remedy in offenders:
        print("{}:{}: zero-argument Actor::ClosestPlayer call".format(rel, lineno))

    # Print each distinct remedy once, so a multi row future stays readable.
    print("")
    print("REMEDY")
    printed = set()
    for _rel, _lineno, symbol, remedy in offenders:
        if symbol in printed:
            continue
        printed.add(symbol)
        print("  {}: {}".format(symbol, remedy))
    return 1


if __name__ == "__main__":
    sys.exit(main())
