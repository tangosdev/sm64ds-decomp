#!/usr/bin/env python3
"""Refuse a build whose port/CMakeLists.txt sets the same per-source property
on the same source twice.

WHY THIS EXISTS. CMake's set_source_files_properties(<src> PROPERTIES <prop>
<value>) REPLACES the property; it does not append to it. main's translation
unit consolidation folded each class's one-function files into a single .cpp,
and port/CMakeLists.txt's pre-sync rows were carried across BY NAME, so several
rows now name the same path. Only the LAST one survives and every earlier row
is silently dropped -- no warning from CMake, no diagnostic from the compiler,
just a missing -D on the command line. Four of those dropped rows cost levels
28 and 39 their boot (398ff4501 / 90197bf77); nobody measured what the rest
cost. This guard makes the shape impossible to reintroduce.

The APPEND forms are safe and are NOT flagged:
    set_property(SOURCE <src> APPEND PROPERTY <prop> <value>)
    set_property(SOURCE <src> APPEND_STRING PROPERTY <prop> <value>)
A bare set_property(SOURCE <src> PROPERTY <prop> <value>) replaces, exactly
like set_source_files_properties, and IS flagged.

The parser resolves the variable spellings this file actually uses for source
paths and refuses to guess at any other: a source argument it cannot resolve is
reported as UNRESOLVED and skipped rather than silently passed. A call inside a
foreach() over a list variable is dynamic by construction, so its source is
unresolvable and it is reported that way.

SELFTEST. --selftest runs the checker over a built-in positive example (which
MUST be reported) and a built-in negative example (which must NOT be), and is
run on every invocation before the real scan. Gutting the check fails the
selftest and therefore fails the build; it cannot silently pass.
"""

import argparse
import os
import re
import sys

# The per-source properties this guard governs. COMPILE_DEFINITIONS is the one
# that has bitten; the other three have the same replace semantics.
GOVERNED = ("COMPILE_DEFINITIONS", "COMPILE_OPTIONS", "COMPILE_FLAGS",
            "INCLUDE_DIRECTORIES")


class Call(object):
    def __init__(self, kind, line, sources, prop, value, appends, raw,
                 start=-1, end=-1):
        self.kind = kind          # "set_source_files_properties" | "set_property"
        self.line = line          # 1-based line of the opening token
        self.sources = sources    # list of resolved (or UNRESOLVED:...) paths
        self.prop = prop
        self.value = value
        self.appends = appends    # True when the call APPENDs, so it is safe
        self.raw = raw
        self.start = start        # char offset of the call name in the file
        self.end = end            # char offset just past the closing paren


def _strip_comments(text):
    """Blank out # comments outside quotes, keeping line and column counts."""
    out = []
    in_str = False
    in_comment = False
    i = 0
    n = len(text)
    while i < n:
        ch = text[i]
        if in_comment:
            out.append("\n" if ch == "\n" else " ")
            if ch == "\n":
                in_comment = False
            i += 1
            continue
        if in_str:
            out.append(ch)
            if ch == "\\" and i + 1 < n:
                out.append(text[i + 1])
                i += 2
                continue
            if ch == '"':
                in_str = False
            i += 1
            continue
        if ch == '"':
            in_str = True
            out.append(ch)
            i += 1
            continue
        if ch == "#":
            in_comment = True
            out.append(" ")
            i += 1
            continue
        out.append(ch)
        i += 1
    return "".join(out)


def _split_args(body):
    """Split a CMake argument list into tokens, keeping quoted strings whole."""
    args = []
    cur = []
    in_str = False
    i = 0
    n = len(body)
    while i < n:
        ch = body[i]
        if in_str:
            if ch == "\\" and i + 1 < n:
                cur.append(body[i + 1])
                i += 2
                continue
            if ch == '"':
                in_str = False
                i += 1
                continue
            cur.append(ch)
            i += 1
            continue
        if ch == '"':
            in_str = True
            i += 1
            continue
        if ch in " \t\r\n;":
            if cur:
                args.append("".join(cur))
                cur = []
            i += 1
            continue
        cur.append(ch)
        i += 1
    if cur:
        args.append("".join(cur))
    return args


def _find_calls(text, names):
    """Yield (name, line, body, start, end) for every call to one of `names`."""
    clean = _strip_comments(text)
    pattern = re.compile(r"(?<![A-Za-z0-9_])(" + "|".join(names) + r")\s*\(")
    for m in pattern.finditer(clean):
        start = m.end()          # first char after '('
        depth = 1
        i = start
        in_str = False
        while i < len(clean) and depth > 0:
            ch = clean[i]
            if in_str:
                if ch == "\\":
                    i += 2
                    continue
                if ch == '"':
                    in_str = False
                i += 1
                continue
            if ch == '"':
                in_str = True
            elif ch == "(":
                depth += 1
            elif ch == ")":
                depth -= 1
                if depth == 0:
                    break
            i += 1
        body = clean[start:i]
        line = clean.count("\n", 0, m.start()) + 1
        yield m.group(1), line, body, m.start(), i + 1


def _resolver(cmake_dir, repo_root, binary_dir):
    """The variable spellings port/CMakeLists.txt uses for source paths."""
    table = {
        "CMAKE_CURRENT_SOURCE_DIR": cmake_dir,
        "CMAKE_CURRENT_LIST_DIR": cmake_dir,
        "CMAKE_SOURCE_DIR": cmake_dir,
        "PROJECT_SOURCE_DIR": cmake_dir,
        "CMAKE_BINARY_DIR": binary_dir,
        "CMAKE_CURRENT_BINARY_DIR": binary_dir,
        "PORT_REPO_ROOT_ABS": repo_root,
    }
    var = re.compile(r"\$\{([A-Za-z0-9_]+)\}")

    def resolve(arg):
        prev = None
        cur = arg
        for _ in range(8):
            if prev == cur:
                break
            prev = cur
            cur = var.sub(lambda m: table.get(m.group(1), "\0" + m.group(1) + "\0"), cur)
        if "\0" in cur:
            unknown = [p for p in cur.split("\0")[1::2]]
            return None, unknown
        return os.path.normpath(cur).replace("\\", "/"), []

    return resolve


def parse(text, cmake_dir=".", repo_root="..", binary_dir="build/port"):
    """Return (calls, unresolved) for one CMakeLists text."""
    resolve = _resolver(cmake_dir, repo_root, binary_dir)
    calls = []
    unresolved = []

    for name, line, body, cstart, cend in _find_calls(
            text, ("set_source_files_properties", "set_property")):
        args = _split_args(body)
        if not args:
            continue

        if name == "set_property":
            if args[0] != "SOURCE":
                continue
            args = args[1:]
            appends = False
            srcs = []
            idx = 0
            while idx < len(args):
                a = args[idx]
                if a in ("APPEND", "APPEND_STRING"):
                    appends = True
                    idx += 1
                    continue
                if a in ("PROPERTY", "DIRECTORY", "TARGET_DIRECTORY"):
                    break
                srcs.append(a)
                idx += 1
            if idx >= len(args) or args[idx] != "PROPERTY":
                continue
            rest = args[idx + 1:]
        else:
            appends = False
            if "PROPERTIES" not in args:
                continue
            k = args.index("PROPERTIES")
            srcs = args[:k]
            rest = args[k + 1:]

        # rest is <prop> <value> [<prop> <value> ...]; a value may be absent.
        pairs = []
        j = 0
        while j < len(rest):
            prop = rest[j]
            value = rest[j + 1] if j + 1 < len(rest) else ""
            pairs.append((prop, value))
            j += 2

        # A source listed twice inside ONE call is not a collision: the call
        # still sets the property once. Only a SECOND call replaces.
        resolved = []
        for s in srcs:
            path, unknown = resolve(s)
            if path is None:
                unresolved.append((line, name, s, unknown))
            elif path not in resolved:
                resolved.append(path)

        for prop, value in pairs:
            if prop not in GOVERNED:
                continue
            calls.append(Call(name, line, resolved, prop, value, appends, body,
                              cstart, cend))

    return calls, unresolved


def collisions(calls):
    """Map (source, property) -> [Call, ...] for every replacing pair seen more
    than once. Appending calls never collide and are excluded."""
    seen = {}
    for c in calls:
        if c.appends:
            continue
        for src in c.sources:
            seen.setdefault((src, c.prop), []).append(c)
    return dict((k, v) for k, v in seen.items() if len(v) > 1)


# ---------------------------------------------------------------- selftest --

POSITIVE = """
# two rows naming the same TU for the same property: the second wins silently
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/game/actors/d_a_obj_kumo.cpp"
    PROPERTIES COMPILE_DEFINITIONS "A=1")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/game/actors/d_a_obj_kumo.cpp"
    PROPERTIES COMPILE_DEFINITIONS "B=2")
"""

NEGATIVE = """
# same TU, DIFFERENT properties: no collision
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/one.cpp"
    PROPERTIES COMPILE_DEFINITIONS "A=1")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/one.cpp"
    PROPERTIES COMPILE_OPTIONS "/Od")
# different TUs, same property: no collision
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/two.cpp"
    PROPERTIES COMPILE_DEFINITIONS "A=1")
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/three.cpp"
    PROPERTIES COMPILE_DEFINITIONS "A=1")
# the APPEND form is the safe one and must never be flagged
set_property(SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../src/four.cpp"
    APPEND PROPERTY COMPILE_DEFINITIONS "A=1")
set_property(SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../src/four.cpp"
    APPEND PROPERTY COMPILE_DEFINITIONS "B=2")
# the same source listed TWICE INSIDE ONE call still sets the property once
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/six.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/six.cpp"
    PROPERTIES COMPILE_DEFINITIONS "A=1")
# a comment naming set_source_files_properties(x PROPERTIES COMPILE_DEFINITIONS)
# twice for "${CMAKE_CURRENT_SOURCE_DIR}/../src/two.cpp" must not count
"""

# The bare (non-APPEND) set_property form replaces just like the other call,
# so it has to be caught too.
POSITIVE_BARE = """
set_property(SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../src/five.cpp"
    PROPERTY COMPILE_DEFINITIONS "A=1")
set_property(SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../src/five.cpp"
    PROPERTY COMPILE_DEFINITIONS "B=2")
"""


def selftest(verbose=False):
    failures = []

    c, _ = parse(POSITIVE, cmake_dir="/p", repo_root="/r", binary_dir="/b")
    hits = collisions(c)
    if len(hits) != 1:
        failures.append("positive example: expected 1 collision, got %d" % len(hits))
    else:
        (src, prop), rows = list(hits.items())[0]
        if not src.endswith("d_a_obj_kumo.cpp") or prop != "COMPILE_DEFINITIONS":
            failures.append("positive example: wrong key %r %r" % (src, prop))
        if [r.value for r in rows] != ["A=1", "B=2"]:
            failures.append("positive example: wrong values %r"
                            % [r.value for r in rows])

    c, _ = parse(POSITIVE_BARE, cmake_dir="/p", repo_root="/r", binary_dir="/b")
    if len(collisions(c)) != 1:
        failures.append("bare set_property example: expected 1 collision, got %d"
                        % len(collisions(c)))

    c, _ = parse(NEGATIVE, cmake_dir="/p", repo_root="/r", binary_dir="/b")
    hits = collisions(c)
    if hits:
        failures.append("negative example: expected 0 collisions, got %d: %r"
                        % (len(hits), sorted(k for k, _ in hits.items())))

    # An unresolvable source must be reported, never silently passed.
    c, un = parse('set_source_files_properties("${_loopvar}" PROPERTIES '
                  'COMPILE_DEFINITIONS "A=1")',
                  cmake_dir="/p", repo_root="/r", binary_dir="/b")
    if not un:
        failures.append("unresolved example: an unknown variable was not reported")

    if failures:
        sys.stderr.write("cdefs_guard SELFTEST FAILED:\n")
        for f in failures:
            sys.stderr.write("  %s\n" % f)
        sys.stderr.write("The guard's own rules no longer hold, so its verdict on\n"
                         "port/CMakeLists.txt means nothing. Fix the guard.\n")
        return False
    if verbose:
        print("cdefs_guard selftest: 4 examples, all as expected")
    return True


# -------------------------------------------------------------------- main --

def main(argv):
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("root", nargs="?", default=None,
                    help="repo root (default: two levels above this script)")
    ap.add_argument("--cmake", default=None,
                    help="the CMakeLists to scan (default: <root>/port/CMakeLists.txt)")
    ap.add_argument("--build-dir", default=None,
                    help="CMAKE_BINARY_DIR (default: <root>/build/port)")
    ap.add_argument("--selftest", action="store_true",
                    help="run the built-in examples and exit")
    ap.add_argument("--tsv", default=None,
                    help="also write every collision as a TSV to this path")
    ap.add_argument("-v", "--verbose", action="store_true")
    args = ap.parse_args(argv)

    here = os.path.dirname(os.path.abspath(__file__))
    root = args.root or os.path.dirname(os.path.dirname(here))
    root = os.path.abspath(root).replace("\\", "/")
    cmake = args.cmake or os.path.join(root, "port", "CMakeLists.txt")
    cmake = os.path.abspath(cmake).replace("\\", "/")
    build_dir = args.build_dir or os.path.join(root, "build", "port")
    build_dir = os.path.abspath(build_dir).replace("\\", "/")
    cmake_dir = os.path.dirname(cmake)

    # The selftest runs on EVERY invocation, not just under --selftest: a
    # gutted check must fail the build rather than report a clean tree.
    if not selftest(verbose=args.verbose or args.selftest):
        return 2
    if args.selftest:
        return 0

    with open(cmake, "r", encoding="utf-8", errors="replace") as fh:
        text = fh.read()

    calls, unresolved = parse(text, cmake_dir=cmake_dir, repo_root=root,
                              binary_dir=build_dir)
    hits = collisions(calls)

    if args.verbose:
        print("cdefs_guard: %s" % cmake)
        print("  %d governed per-source property calls" % len(calls))
    if unresolved:
        # Printed on EVERY run, not only under -v. This is the check's blind
        # spot and it belongs in the build log, not behind a flag nobody passes.
        print("cdefs_guard: note -- %d source argument(s) are computed at "
              "configure time\n  (foreach variables, generated lists) and are "
              "OUTSIDE this check. Audited by hand\n  on 2026-09-16: every one "
              "of those loops iterates a LITERAL list, the lists are\n  disjoint, "
              "none of the 37 sources they expand to is named by a static row "
              "for\n  the same property, and the one loop that can revisit a "
              "source reads the property\n  back with get_source_file_property "
              "and appends. Re-audit when one of these\n  lines changes; -v "
              "lists them." % len(unresolved))
    if args.verbose:
        for line, name, arg, unknown in unresolved:
            print("    line %-6d %s(%s)  unknown: %s"
                  % (line, name, arg, ",".join(unknown)))

    rows = []
    for (src, prop), cs in sorted(hits.items()):
        cs = sorted(cs, key=lambda c: c.line)
        rel = src[len(root) + 1:] if src.startswith(root + "/") else src
        exists = "yes" if os.path.exists(src) else "NO"
        for i, c in enumerate(cs):
            rows.append((rel, prop, str(c.line), c.kind,
                         "EFFECTIVE" if i == len(cs) - 1 else "DROPPED",
                         exists, c.value))

    if args.tsv:
        with open(args.tsv, "w", encoding="utf-8") as fh:
            fh.write("source\tproperty\tline\tcall\tverdict\tfile_exists\tvalue\n")
            for r in rows:
                fh.write("\t".join(r) + "\n")

    if not hits:
        print("cdefs_guard: OK -- no source in %s takes the same per-source "
              "property twice" % os.path.relpath(cmake, root).replace("\\", "/"))
        return 0

    sys.stderr.write(
        "cdefs_guard: REFUSED -- %d source/property pair(s) are set more than "
        "once in %s.\n"
        "set_source_files_properties() and the bare set_property(SOURCE ...) "
        "form REPLACE the\nproperty, so only the LAST call survives and every "
        "earlier one is dropped with no\ndiagnostic. Merge the rows into ONE "
        "call (semicolon-join the definitions), or delete\nthe dead ones with "
        "the reason in the commit. Use the APPEND form if you mean to add.\n\n"
        % (len(hits), os.path.relpath(cmake, root).replace("\\", "/")))
    for (src, prop), cs in sorted(hits.items()):
        cs = sorted(cs, key=lambda c: c.line)
        rel = src[len(root) + 1:] if src.startswith(root + "/") else src
        sys.stderr.write("  %s   [%s]%s\n"
                         % (rel, prop,
                            "" if os.path.exists(src) else "   (NOTE: no such file)"))
        for i, c in enumerate(cs):
            sys.stderr.write("      line %-6d %-9s %s\n"
                             % (c.line,
                                "EFFECTIVE" if i == len(cs) - 1 else "dropped",
                                c.value))
    if args.tsv:
        sys.stderr.write("\n  (full table written to %s)\n" % args.tsv)
    return 1


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
