#!/usr/bin/env python3
"""Demangle MSVC decorated names with undname, correctly.

WHY THIS IS ITS OWN MODULE, AND WHY IT LEADS WITH A BUG REPORT

    undname.exe takes FILENAMES, never stdin:

        Usage: undname [flags] fname [fname...]
           or: undname [flags] file

    Pipe a list of names into it and it prints that usage banner to stdout and
    exits 1. It does not read the pipe. That is a perfectly ordinary CLI
    contract and it produced a bad day here: port/tools/aliascheck.py shipped
    with a helper that did exactly that, treated the non-zero exit as "the tool
    is missing", and reported

        RULE P NOT RUN: undname is not on PATH

    in EVERY environment, including this one, where undname sits at
    ...\\VC\\Tools\\MSVC\\14.44.35207\\bin\\Hostx86\\x86\\undname.exe and works
    fine. A whole rule of a checker read as a blocked external dependency for a
    day because of the invocation shape. Lane ABR1's review caught it.

    So the invocation lives in one place, both checkers call it, and its
    selftest pins the FILE form against known output. A second copy of this
    would be a second chance to pipe into it.

WHAT IT GUARANTEES
    undname emits exactly one line per input line, in order. If it does not,
    the alignment is a guess, and a guessed alignment is how a checker reports
    a defect against the wrong symbol. demangle() returns {} rather than
    guessing, and says why on stderr.

FINDING IT
    PATH first. Then the VS Build Tools layout, because the callers run from
    plain shells (a git hook, a CMake custom target, a lane's terminal) where
    nobody ran vcvars32 -- and requiring vcvars to get a rule is the same
    availability problem in a different coat.

    On a machine with no MSVC at all -- a Linux CI runner, for instance --
    find_undname() returns None and every caller reports the rule as NOT RUN.
    That is the honest answer and it is never a pass.
"""

import glob
import os
import shutil
import subprocess
import sys
import tempfile

_CACHE = []


def find_undname():
    """undname.exe's path, or None."""
    if _CACHE:
        return _CACHE[0]
    found = shutil.which("undname")
    if not found:
        hits = []
        for var in ("ProgramFiles(x86)", "ProgramFiles"):
            base = os.environ.get(var)
            if not base:
                continue
            hits.extend(glob.glob(os.path.join(
                base, "Microsoft Visual Studio", "*", "*", "VC", "Tools",
                "MSVC", "*", "bin", "Host*", "*", "undname.exe")))
        # Highest MSVC toolset version last after a plain sort of the paths;
        # any of them demangles identically, so this only needs to be stable.
        found = sorted(hits)[-1] if hits else None
    _CACHE.append(found)
    return found


def demangle(names, quiet=False):
    """{decorated: demangled} for every '?'-prefixed name, or {} on failure."""
    names = [n for n in names if n.startswith("?")]
    if not names:
        return {}
    exe = find_undname()
    if exe is None:
        return {}
    fd, path = tempfile.mkstemp(prefix="undname_", suffix=".txt", text=True)
    try:
        with os.fdopen(fd, "w", encoding="utf-8", newline="\n") as f:
            f.write("\n".join(names) + "\n")
        try:
            p = subprocess.run([exe, path], capture_output=True, text=True,
                               timeout=300)
        except (OSError, subprocess.SubprocessError) as e:
            if not quiet:
                sys.stderr.write("undname: %s\n" % e)
            return {}
    finally:
        try:
            os.remove(path)
        except OSError:
            pass
    if p.returncode != 0:
        if not quiet:
            sys.stderr.write("undname exited %d\n%s\n"
                             % (p.returncode, p.stdout[:400]))
        return {}
    lines = [ln for ln in p.stdout.splitlines() if ln.strip()]
    if len(lines) != len(names):
        if not quiet:
            sys.stderr.write(
                "undname gave %d lines for %d names; refusing to guess the "
                "alignment\n" % (len(lines), len(names)))
        return {}
    return dict(zip(names, lines))


# Known-good pairs. Chosen to cover every decoration form the port's checkers
# read a convention out of: public thiscall, thiscall with an argument, virtual
# thiscall, a static member (cdecl, no receiver), and a data symbol.
SELFTEST_PAIRS = [
    ("?SetDefault@Heap@@QAEHXZ",
     "public: int __thiscall Heap::SetDefault(void)"),
    ("?TryGrab@Player@@QAE_NAAUActor@@@Z",
     "public: bool __thiscall Player::TryGrab(struct Actor &)"),
    ("?AfterBehavior@fBase_c@@UAEXI@Z",
     "public: virtual void __thiscall ActorBase::AfterBehavior(unsigned int)"),
    ("?LoadInitialGroup@Sound@@SAXH@Z",
     "public: static void __cdecl Sound::LoadInitialGroup(int)"),
    ("?IDENTITY_MATRIX4X3@@3UM48@@A", "struct M48 IDENTITY_MATRIX4X3"),
]


def selftest():
    """0 pass, 1 fail, 2 undname unavailable (reported, never a pass)."""
    exe = find_undname()
    print("  undname: %s" % (exe or "NOT FOUND"))
    if exe is None:
        print("    SKIPPED, not failed: no undname on PATH and none under the "
              "VS Build Tools layout. Every rule that needs it reports NOT "
              "RUN rather than passing.")
        return 2
    got = demangle([d for d, _ in SELFTEST_PAIRS])
    bad = 0
    for dec, want in SELFTEST_PAIRS:
        g = got.get(dec, "<missing>")
        ok = g == want
        bad += 0 if ok else 1
        print("    %-4s %s" % ("ok" if ok else "FAIL", dec))
        if not ok:
            print("         want %s" % want)
            print("         got  %s" % g)
    # The bug this module exists for: undname does NOT read stdin. Pin it, so
    # nobody reintroduces the pipe on the grounds that it looks tidier.
    try:
        p = subprocess.run([exe], input="?SetDefault@Heap@@QAEHXZ\n",
                           capture_output=True, text=True, timeout=60)
        stdin_refused = p.returncode != 0 or "Usage:" in p.stdout
    except (OSError, subprocess.SubprocessError):
        stdin_refused = True
    print("    %-4s undname refuses stdin (exit %s), so the FILE form is the "
          "only correct call" % ("ok" if stdin_refused else "FAIL",
                                 p.returncode if 'p' in dir() else "?"))
    bad += 0 if stdin_refused else 1
    return 1 if bad else 0


if __name__ == "__main__":
    print("msvc_undname --selftest")
    rc = selftest()
    print("SELFTEST %s" % {0: "PASSED", 1: "FAILED",
                           2: "SKIPPED (undname unavailable)"}[rc])
    sys.exit(0 if rc != 1 else 1)
