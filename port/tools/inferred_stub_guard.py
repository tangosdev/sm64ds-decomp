"""Build time RATCHET against NEW inferred vtable stub bodies getting seated.

Some src/ bodies were never decompiled from the ROM. They were guessed from a
vtable slot: the tool saw a class had a virtual in slot N, knew the address the
ROM parked there, and wrote a plausible C body for it. Every such body carries
the marker line

    recovered from vtable slot identity

in a header comment. There are 134 of these markers in src/ today and 132 of them
are in the build's active TU set. They are behavioral GUESSES, not verified
decompilations, so a body that looks right can still be wrong in a way no byte
gate on the ROM catches, because the ROM never built from this source.

(This paragraph said 712 markers and 143 seated from the day it was written until
2026-09-15, and the project memory note copied those figures out of it. Neither
was ever measured against the tree. Lane STUBS counted the files and got 134, and
lane STUBGUARD reproduced that count and the 132. If you change the seating test,
re-measure these and rewrite them. A stale number in a guard's own docstring is
how a guard quietly stops being read.)

A guessed body only affects the port once it is SEATED: once something the build
compiles can dispatch into it. A seated guess runs. An unseated guess is dead
source. 132 are seated today.

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

  2. its @symbol is named, outside comments, by something the build compiles, on
     any of the FOUR routes below.

A file that is inferred but unseated (marker present, but not in the active TU
set, or on no route) is latent and does not count. That is the set the port is
meant to work DOWN: seat none of them anew, and retire the seated ones as they
get truly decompiled.

THE FOUR ROUTES, and why there are four (rewritten 2026-09-15, lane STUBGUARD).

Until today this guard tested ONE route, a port/hal reference, with ONE reader
that could only recognise names spelled func_ovNNN_ADDR or func_ADDR. It
reported 9 seated bodies when 132 were live: it was missing fourteen out of
every fifteen. Three separate blind spots produced that number, and each of them
is the same mistake in a different coat, which is why the fix is not "add the
missing route" but "stop deciding what to look for by guessing its shape":

  a. NAME SHAPE. The reader tokenised hal files with a regex for func_ names, so
     a body whose @symbol is an Itanium name (_ZN10LavaSeesaw6RenderEv) or a
     plain factory name (daPkn_c_classInit) could never match, even when a hal
     class table named it on the very next line. All 9 bodies the guard could
     see were func_ names. That was not a coincidence, that was the bug.
  b. FILE EXTENSION. The hal scan globbed .c, .cpp and .h. port/hal holds five
     .inc files, and actor_classes.inc carries class table rows. Never opened.
  c. DISPATCH ROUTE. A hal vtable fill is only one of the ways the ROM reaches a
     body, and by now it is the minority way.

So the reference test no longer tokenises anything. The marker symbols are known
exactly, from the @symbol lines, and each route is searched for THOSE STRINGS as
whole words with comments blanked. A name that changes shape cannot hide again.

  FACE  port/faces_sync.txt, the reviewed face ledger. A face is a generated
        extern "C" forwarder that lets a ROM vtable dispatch into a C++ member
        body with no hal file involved; there are about 1900 of them and 95
        marker bodies sit behind one. The ledger is read, not the generated
        faces_sync_gen.cpp, because that file does not exist yet when this guard
        runs (ahead of cmake configure) and because the ledger is authoritative
        by its own rule: facegen will not generate a name that is not in it.
        An R or F row's first column is the flat ROM name, carrying MSVC's
        leading underscore, so one underscore is stripped. A D row's second
        column is the D1 body its D0 face runs.
  HAL   port/hal/**, now including .inc. A class table naming the symbol.
  PORT  any other port/ TU: state dispatch tables and factories that hold the
        body's address (port/unmatched/*_StateDispatch.cpp and friends).
  SRC   any other active decomp TU that names it, which is a direct ROM call.

A reference from a port HEADER is not treated specially and does not need to be:
no port header names a marker symbol at all today. If that ever changes, a bare
declaration would read as a seat, which errs toward reporting MORE seats. For a
ratchet that is the safe direction, and it is stated here so the next reader
knows it was a decision and not an oversight.

Wired ahead of cmake configure in build-port.cmd, next to closestplayer_guard.

SELF-EVIDENCING (the house standard as of tonight). Every run prints how many
files each route scanned and how many seats it found, so a reader can see the
guard is awake instead of trusting that it is. A route whose input has vanished
(no hal directory, no face ledger, no src tree) is not silently zero: it FAILS
the run. That is the property that was missing before, when a route the guard
could not read was indistinguishable from a route with nothing on it.

LIMITATION, unchanged: the active TU set is read statically from
port/slice_gate*.txt and the literal source paths in port/CMakeLists.txt. A
source pulled in through a path the reader cannot resolve statically (a
generated file, a glob, a computed variable) is not seen. Note this limitation
now only narrows the CANDIDATE side (which marker bodies count as live). The
REFERENCE side deliberately scans the whole port/ and src/ trees rather than the
resolved TU set, so a seating cannot hide behind a CMake path this reader cannot
follow.
"""

import concurrent.futures
import os
import re
import sys

MARKER = "recovered from vtable slot identity"

# The symbol a marker file exports, one per file, on its `// @symbol NAME` line.
# There is deliberately NO regex here describing what such a name looks like.
# The old SYMBOL_TOKEN pattern matched func_ovNNN_ADDR and func_ADDR only, and
# every Itanium name and every plain factory name was invisible to this guard
# for as long as it existed. Names are read from the @symbol lines and then
# searched for literally; the tool never decides what a symbol looks like.
SYMBOL_LINE = re.compile(r"^//\s*@symbol\s+(\S+)")

# Source extensions that can hold a seating. .inc is here because port/hal keeps
# class table rows in actor_classes.inc, which the old .c/.cpp/.h glob skipped.
CODE_EXT = (".c", ".cpp", ".cc", ".h", ".hpp", ".inc")

# Comment bodies, for blanking. See blank_comments.
COMMENT_RUN = re.compile(r"//[^\n]*|/\*.*?(?:\*/|\Z)", re.S)

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


def _blank_run(match):
    run = match.group(0)
    return "".join(c if c == "\n" else " " for c in run)


def blank_comments(src):
    """Replace C and C++ comment bodies with spaces, preserving newlines.

    A hal fill or a marker file documents symbols in prose; that prose must not
    count as a reference or as a live symbol line. Newlines are kept so nothing
    downstream that cares about line numbers is thrown off.

    This was a character at a time loop while it only ever saw the handful of
    files in port/hal. The reference scan now covers the whole port/ and src/
    trees, about 14 MB, and the loop took minutes on that. The regex does the
    same job: re.sub takes the LEFTMOST match, so at any position the first of
    // and /* to open still wins, exactly as the loop's lookahead did, and the
    \\Z arm reproduces the loop's handling of a comment that is never closed.
    Proven equal to the old implementation over every .c, .cpp, .h and .inc file
    in port/ and src/ (9589 files) before it was swapped in.
    """
    return COMMENT_RUN.sub(_blank_run, src)


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
    # EVERY slice file, not just the gate ones. The reader matched
    # "slice_gate*.txt" only, so it was blind to every lane slice the linkage
    # campaign has written (slice_w*.txt) -- and by run linkw wave 12 that was
    # the large majority of the sliced tree. The guard reported its 2/2
    # baseline the whole time while seated bodies it exists to catch sat in
    # files it never opened. A guard that cannot see most of the build is not
    # a ratchet, it is a decoration.
    gate_names = sorted(
        n for n in os.listdir(port_dir)
        if n.startswith("slice_") and n.endswith(".txt")
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


class BlindRoute(Exception):
    """A seating route could not be read at all.

    Raised instead of quietly returning nothing. A route whose input directory
    or ledger has moved, been renamed or been deleted used to be
    indistinguishable from a route with no seats on it, and that is precisely
    how this guard spent its whole life reporting 9 when the answer was 132.
    """


def _code_files(root):
    """Every source file under root that could hold a seating, sorted."""
    out = []
    for base, _dirs, names in os.walk(root):
        for name in names:
            if name.endswith(CODE_EXT):
                out.append(os.path.join(base, name))
    out.sort()
    return out


def symbol_search_re(symbols):
    """One regex matching any of symbols as a whole word.

    Whole word means not preceded or followed by an identifier character, so
    func_ov002_020ba4c0 does not match inside func_ov002_020ba4c0_hostcopy.
    Longest first so an alternation never stops at a shorter prefix.
    """
    if not symbols:
        return None
    body = "|".join(re.escape(s) for s in sorted(symbols, key=len,
                                                 reverse=True))
    return re.compile(r"(?<![A-Za-z0-9_])(?:" + body + r")(?![A-Za-z0-9_])")


def references_in(paths, pattern, label):
    """symbol -> set of repo relative files naming it outside comments.

    paths is a list of (rel, abs) pairs. Reads through the same thread pool the
    marker scan uses, because this is I/O bound for the same reason.
    """
    if not paths:
        raise BlindRoute("route {} had no files to scan".format(label))
    if pattern is None:
        return {}

    def _one(item):
        rel, abs_path = item
        try:
            with open(abs_path, "r", encoding="utf-8",
                      errors="replace") as fh:
                text = fh.read()
        except OSError:
            return rel, ()
        return rel, tuple(m.group(0)
                          for m in pattern.finditer(blank_comments(text)))

    found = {}
    with concurrent.futures.ThreadPoolExecutor(
            max_workers=_read_workers()) as pool:
        for rel, hits in pool.map(_one, paths):
            for sym in hits:
                found.setdefault(sym, set()).add(rel)
    return found


def face_ledger_symbols(port_dir):
    """Every body symbol a generated face dispatches into.

    port/faces_sync.txt is read rather than the generated faces_sync_gen.cpp
    because that file does not exist when this guard runs, ahead of cmake
    configure, and because the ledger is authoritative by its own rule: facegen
    will not generate a name that is not a row here.

    Columns are  <flat ROM name>  <decorated member>  <address>  <R|F|D ...>.
    For an R row the face defines the flat name and calls the member, so the
    flat name IS the body, spelled the way a marker file's @symbol spells it
    once MSVC's single leading underscore is stripped. For an F row the face
    defines the member and calls the flat ROM body, so the flat name is the body
    again. A D row's second column is the D1 body its D0 face runs, already
    written flat.
    """
    path = os.path.join(port_dir, "faces_sync.txt")
    if not os.path.isfile(path):
        raise BlindRoute("face ledger missing at port/faces_sync.txt")
    syms = set()
    rows = 0
    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        for raw in fh:
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            if len(parts) < 4:
                continue
            rows += 1
            flat = parts[0]
            if flat.startswith("__Z"):
                flat = flat[1:]
            syms.add(flat)
            if parts[3] == "D":
                syms.add(parts[1])
    if not rows:
        raise BlindRoute("face ledger port/faces_sync.txt holds no rows")
    return syms, rows


def _read_workers():
    """How many files to have open at once.

    This scan is not compute: profiling it on this tree put 113 of its 122
    seconds inside _io.open, across roughly ten thousand source files, at about
    eleven milliseconds an open -- the on-access virus scanner and ten other
    lanes hammering the same disk. Opens release the GIL, so threads are the
    right lever and the count wants to be well past the core count. Nothing
    about WHAT is checked changes: the pool preserves input order, so the
    reported offenders come out in exactly the order the serial loop produced.
    """
    return min(16, (os.cpu_count() or 4) * 2)


def live_marker_bodies(port_dir, repo_dir):
    """symbol -> repo relative path, for every marker file the build compiles.

    The CANDIDATE side. Restricted to the active TU set on purpose: a marker
    file the build does not compile is latent and is not a seat.
    """
    tus = [(rel, abs_path)
           for rel, abs_path in build_tu_set(port_dir, repo_dir)
           if rel.startswith("src/")]
    if not tus:
        raise BlindRoute("no src/ TUs in the active build set")

    def _one(item):
        rel, abs_path = item
        with open(abs_path, "r", encoding="utf-8", errors="replace") as fh:
            text = fh.read()
        if MARKER not in text:
            return None
        sym = marker_symbol(abs_path)
        return (sym, rel) if sym else None

    bodies = {}
    with concurrent.futures.ThreadPoolExecutor(
            max_workers=_read_workers()) as pool:
        for got in pool.map(_one, tus):
            if got is not None:
                bodies[got[0]] = got[1]
    return bodies


def marker_symbols_anywhere(repo_dir):
    """symbol -> repo relative path for EVERY marker file under src/, compiled
    or not. Used only to explain why a symbol left the seated set, so it is
    called on the rare runs that have something to explain rather than on every
    run."""
    src_dir = os.path.join(repo_dir, "src")
    files = [p for p in _code_files(src_dir)
             if p.endswith((".c", ".cpp", ".cc"))]

    def _one(abs_path):
        with open(abs_path, "r", encoding="utf-8", errors="replace") as fh:
            text = fh.read()
        if MARKER not in text:
            return None
        sym = marker_symbol(abs_path)
        if not sym:
            return None
        return sym, os.path.relpath(abs_path, repo_dir).replace(os.sep, "/")

    found = {}
    with concurrent.futures.ThreadPoolExecutor(
            max_workers=_read_workers()) as pool:
        for got in pool.map(_one, files):
            if got is not None:
                found[got[0]] = got[1]
    return found


def seating_survey(port_dir, repo_dir):
    """Measure every seating route and return the evidence, not just a number.

    Returns (bodies, routes, scanned) where bodies is symbol -> defining file,
    routes is route name -> {symbol: set(files naming it)} and scanned is route
    name -> how many files that route read. main() prints all of it, because a
    guard that reports a bare count is a guard nobody can tell is awake.

    The REFERENCE side walks the port/ and src/ trees directly rather than the
    statically resolved TU set. The TU reader cannot follow a CMake glob or a
    computed variable, and a seating hiding behind one of those is exactly the
    class of miss this rewrite exists to end.
    """
    bodies = live_marker_bodies(port_dir, repo_dir)
    pattern = symbol_search_re(set(bodies))

    hal_dir = os.path.join(port_dir, "hal")
    if not os.path.isdir(hal_dir):
        raise BlindRoute("no port/hal directory")
    src_dir = os.path.join(repo_dir, "src")
    if not os.path.isdir(src_dir):
        raise BlindRoute("no src/ directory")

    def pairs(files):
        return [(os.path.relpath(p, repo_dir).replace(os.sep, "/"), p)
                for p in files]

    hal_files = pairs(_code_files(hal_dir))
    hal_set = set(rel for rel, _ in hal_files)
    port_files = [item for item in pairs(_code_files(port_dir))
                  if item[0] not in hal_set]
    src_files = pairs(_code_files(src_dir))

    face_syms, face_rows = face_ledger_symbols(port_dir)

    routes = {}
    scanned = {}

    routes["face"] = dict(
        (sym, {"port/faces_sync.txt"})
        for sym in sorted(set(bodies) & face_syms))
    scanned["face"] = face_rows

    for label, files in (("hal", hal_files), ("port", port_files),
                         ("src", src_files)):
        hits = references_in(files, pattern, label)
        # A body naming itself is a definition, not a seating.
        routes[label] = dict(
            (sym, files_naming)
            for sym, files_naming in hits.items()
            if sym in bodies
            for files_naming in [files_naming - {bodies[sym]}]
            if files_naming)
        scanned[label] = len(files)

    return bodies, routes, scanned


def live_seated_set(port_dir, repo_dir):
    """The current seated inferred stub symbols, sorted. Kept for callers that
    want the set and nothing else; seating_survey carries the evidence."""
    _bodies, routes, _scanned = seating_survey(port_dir, repo_dir)
    seated = set()
    for hits in routes.values():
        seated.update(hits)
    return sorted(seated)


def debt_paths():
    """Every discovered-debt queue file, oldest first.

    There are two, and there is one per DISCOVERY rather than one file that
    grows, because each file's header forbids additions to itself and that rule
    is worth keeping literal:

      inferred_stub_debt.txt   2026-08-15, when the slice reader was widened
                               from slice_gate*.txt to slice_*.txt.
      inferred_stub_debt2.txt  2026-09-15, when the seating test was widened
                               from one route to four.

    Both are the same kind of thing: bodies that were already seated and already
    shipping, made visible by the guard learning to see, never by anyone seating
    them. A third widening gets a third file, not an edit to these.
    """
    here = os.path.dirname(os.path.abspath(__file__))
    return sorted(
        os.path.join(here, name)
        for name in os.listdir(here)
        if name.startswith("inferred_stub_debt") and name.endswith(".txt"))


def debt_path():
    """Kept for callers that want the first queue by name."""
    return os.path.join(os.path.dirname(os.path.abspath(__file__)),
                        "inferred_stub_debt.txt")


def read_debt():
    """The union of the discovered-debt queues: seated guesses that were ALREADY
    shipping and became visible only when this guard learned to see them. Read
    exactly like the baseline, but it is NOT an allowlist -- see each file's own
    header. It may only shrink, and a symbol never enters it: anything seated
    after those files were written is a genuine new guess and fails."""
    syms = []
    for path in debt_paths():
        with open(path, "r", encoding="utf-8", errors="replace") as fh:
            for raw in fh:
                line = raw.strip()
                if not line or line.startswith("#"):
                    continue
                syms.append(line)
    return sorted(set(syms))


def adjudicated_path():
    return os.path.join(os.path.dirname(os.path.abspath(__file__)),
                        "inferred_stub_adjudicated.txt")


def read_adjudicated():
    """Bodies a lane disassembled out of the ROM, compared instruction for
    instruction with their src/ body, ruled REAL DECOMP, and only THEN seated.

    Unlike the baseline and the debt queue, this list is allowed to grow -- but
    only on evidence, and the parser is the thing that enforces it. A line must
    read `SYMBOL REAL_DECOMP EVIDENCE...`; a bare symbol, a symbol with any
    other ruling, and a symbol with no evidence reference are all IGNORED. So a
    failing build cannot be unblocked by adding a name, which is the property
    inferred_stub_debt.txt's header protects by forbidding additions outright.

    The marker this guard keys on records how a function's NAME was recovered,
    not where its BODY came from -- lane w13 ruled all 43 then-queued bodies
    REAL DECOMP, 0 guesses. A per-body ROM comparison answers the question the
    marker only proxies for, so a symbol excused here cleared a strictly higher
    bar than one sitting on the baseline."""
    path = adjudicated_path()
    if not os.path.isfile(path):
        return []
    syms = []
    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        for raw in fh:
            line = raw.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            if len(parts) < 3:
                continue
            if parts[1] != "REAL_DECOMP":
                continue
            syms.append(parts[0])
    return sorted(set(syms))


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
    "# whose @symbol is in the port build's active TU set and which something the\n"
    "# build compiles can dispatch into, on any of the four routes the guard\n"
    "# measures (face ledger, port/hal fill, other port TU, other decomp TU).\n"
    "# These are behavioral guesses, not ROM verified decompilations. This list is\n"
    "# a RATCHET: it can only SHRINK.\n"
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


ROUTE_BLURB = {
    "face": "generated ROM faces (port/faces_sync.txt rows)",
    "hal": "port/hal class table fills (files scanned)",
    "port": "other port TUs, dispatch tables and factories (files scanned)",
    "src": "other decomp TUs calling it directly (files scanned)",
}


def print_survey(bodies, routes, scanned, unrouted):
    """The evidence block. Printed on every run, pass or fail.

    The point is that a reader can see WHAT was counted and BY WHICH ROUTE. A
    check that only ever prints a number cannot be told apart from a check that
    has quietly gone blind, and this one printed 9 for months while the answer
    was 132.
    """
    print("inferred_stub_guard: seating survey")
    print("  guess-marked bodies the build compiles ... {}".format(len(bodies)))
    for label in ("face", "hal", "port", "src"):
        print("    route {:<5} {:>4} seated   [{}: {}]".format(
            label, len(routes.get(label, {})), ROUTE_BLURB[label],
            scanned.get(label, 0)))
    print("  seated on at least one route ............. {}".format(
        len(bodies) - len(unrouted)))
    if unrouted:
        print("  compiled but on NO route found .......... {}".format(
            len(unrouted)))
        for sym in sorted(unrouted):
            print("    unrouted: {}  ({})".format(sym, bodies[sym]))


def main(argv):
    update = "--update" in argv[1:]
    port_dir = port_root()
    repo_dir = repo_root()

    try:
        bodies, routes, scanned = seating_survey(port_dir, repo_dir)
    except BlindRoute as exc:
        # A route that cannot be read is a FAILURE, never a zero. This is the
        # whole lesson: silence and "nothing found" looked identical here for
        # the guard's entire life.
        print("inferred_stub_guard FAIL: a seating route went blind: {}. "
              "Fix the route before trusting any count this tool prints."
              .format(exc))
        return 1

    seated = set()
    for hits in routes.values():
        seated.update(hits)
    live = sorted(seated)
    unrouted = set(bodies) - seated
    print_survey(bodies, routes, scanned, unrouted)
    print("")

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
    debt_set = set(read_debt())
    adj_set = set(read_adjudicated())
    # Debt is carried separately from the baseline and is never an approval:
    # these were already seated and already shipping when the reader was
    # widened to see them. They are reported every run and must reach zero.
    # Adjudicated symbols are the opposite case: ruled against the ROM FIRST,
    # seated second, and admitted only on a recorded ruling (read_adjudicated
    # drops any line that does not carry one).
    new_seated = sorted(live_set - base_set - debt_set - adj_set)
    retired = sorted(base_set - live_set)
    debt_paid = sorted(debt_set - live_set)

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

    print("inferred_stub_guard: {} seated inferred stubs live, {} on baseline, "
          "{} unadjudicated DEBT, {} ROM-ADJUDICATED."
          .format(len(live_set), len(base_set), len(debt_set & live_set),
                  len(adj_set & live_set)))
    if adj_set & live_set:
        print("  ADJUDICATED: {} seated body/bodies carry the marker but were "
              "disassembled from the ROM and ruled REAL DECOMP before being "
              "seated (see inferred_stub_adjudicated.txt). Each still owes a "
              "decomp-side marker correction; the entry retires when the "
              "marker line goes."
              .format(len(adj_set & live_set)))
    if debt_set & live_set:
        print("  DEBT: {} seated guess-marked bodies that were already shipping "
              "when this guard learned to see them (see {}). Each needs a "
              "per-body ruling: real decomp gets its marker corrected and moves "
              "to inferred_stub_adjudicated.txt, a genuine guess gets de-seated. "
              "This is not an approval and the list may only shrink."
              .format(len(debt_set & live_set),
                      ", ".join(os.path.basename(p) for p in debt_paths())))
    if debt_paid or retired:
        # WHY a symbol left the seated set decides whether it is safe to delete
        # the row. Marker gone means a real decomp landed and the guess is
        # retired for good. Marker still present means the body is still a
        # guess and merely stopped being reachable, which a later seating can
        # undo. Saying only "it left" is how a reader is invited to delete a
        # row that still has something behind it.
        still_guessed = marker_symbols_anywhere(repo_dir)
    if debt_paid:
        print("  DEBT PAID: {} symbol(s) left the seated set; tighten the debt "
              "queue by deleting them, but read the reason on each line first:"
              .format(len(debt_paid)))
        for sym in debt_paid:
            if sym in still_guessed:
                print("    {}  STILL GUESS-MARKED at {} (de-seated, not "
                      "decompiled: keep watching it)"
                      .format(sym, still_guessed[sym]))
            else:
                print("    {}  marker gone (real decomp landed)".format(sym))

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
            if sym in still_guessed:
                print("  retired: {}  STILL GUESS-MARKED at {} (de-seated, "
                      "not decompiled)".format(sym, still_guessed[sym]))
            else:
                print("  retired: {}  marker gone (real decomp landed)"
                      .format(sym))

    print("inferred_stub_guard OK: no new guessed vtable body seated.")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
