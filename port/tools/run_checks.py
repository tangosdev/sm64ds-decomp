#!/usr/bin/env python3
"""run_checks -- fire the port's ABI defect detectors and fail on any red.

    python port/tools/run_checks.py <repo-root>
    python port/tools/run_checks.py <repo-root> --abicheck --disasm-dir <dir>

WHY A WRAPPER EXISTS AT ALL
    Five detectors cover the calling-convention / receiver / vtable-shape
    defect class in the PC port. Each is a real gate on its own. The wrapper
    exists because of what they do on EMPTY INPUT and BROKEN FIXTURES, not
    because running five commands is hard.

      aliascheck        an /alternatename that crosses the receiver boundary
                        (flat C one side, __thiscall the other), or whose two
                        sides pop different amounts.  No build.  GATES.
      vtspan --sweep    a host vtable array shorter than its ROM span.  No
                        build.  RATCHETED against vtspan_sweep_baseline.txt.
      vtspan --fills    a fill loop that leaves a ROM slot unwritten.  No
                        build.  GATES.
      aritycheck        a declaration and a definition of the same extern "C"
                        symbol that disagree about how many parameters it
                        takes.  No build.  The full census never fails (see
                        its header for the measured reason); TWO subsets are
                        RATCHETED and do gate: the _ZN receiver shape and the
                        plain-name (func_*) called-prototype shape.
      abicheck          a vtable thunk whose emitted `ret <n>` does not match
                        the pop its Actor slot demands.  Needs a build and a
                        `dumpbin /disasm:nobytes` of the hal objects.  GATES.

THREE WAYS TO READ GREEN WITHOUT CHECKING ANYTHING, AND WHAT IS DONE ABOUT EACH

  1. THE CHECKER'S OWN FIXTURES ARE BROKEN.  A regex that stopped matching a
     shape reports a clean tree in exactly the same words as a clean tree.
     So STAGE 0 runs every checker's --selftest first and refuses to go on if
     any of them fails.  This is the same rule build-port.cmd already applies
     to closure, facegen, mapdiff, vtablerows, stategen and the three guards.

  2. THE INPUT IS EMPTY OR IN THE WRONG FORMAT.  abicheck exits 0 when it
     parses zero vtable slot fills, which is what plain `dumpbin /disasm`
     produces (the encoded bytes sit between the address and the mnemonic and
     the store pattern never matches).  abicheck now refuses a zero-fill run
     itself, and this wrapper checks the count in its output as well, because
     two independent readers of the same failure is the point.

  3. THE SCAN WALKED THE WRONG TREE.  aliascheck refuses a run that finds
     fewer than a hundred /alternatename directives; the port carries over
     fifteen hundred.

    Exit code is 0 only if every gating checker ran, checked something, and
    passed.  Any red, any vacuous pass, any missing input, any broken fixture
    is a non-zero exit.

THE DISASSEMBLY DIRECTORY IS NOT A FIXED PATH.  The version of this recovered
from branch port-defect-wiring guarded a hardcoded C:\\tmp\\sm64ds-abisweep\\
_disasm because the abicheck of the day read that constant.  Two lanes running
the suite at once would have written each other's input.  --disasm-dir is now
passed through, and the CMake target defaults it inside the build directory.
"""

import argparse
import os
import re
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
PY = sys.executable
SWEEP_BASELINE = os.path.join(HERE, "vtspan_sweep_baseline.txt")
FILLS_BASELINE = os.path.join(HERE, "vtspan_fills_baseline.txt")

# Every checker whose fixtures must pass before any of them is believed.
# msvc_undname is in the list even though it is not a checker: aliascheck's
# rule P and abicheck's tail-jump resolution both rest on it, and it spent a
# day silently returning nothing because it piped into undname's stdin. Its
# selftest reports SKIPPED where undname is genuinely absent, which is a real
# state on a Linux runner and is never counted as a pass.
SELFTESTS = ("msvc_undname.py", "aliascheck.py", "abicheck.py",
             "aritycheck.py")


def load_baseline(path):
    """A ratchet file: bare names, strip-at-#, blank lines ignored."""
    allowed = set()
    if os.path.isfile(path):
        for ln in open(path, encoding="utf-8"):
            ln = ln.split("#", 1)[0].strip()
            if ln:
                allowed.add(ln)
    return allowed


def sweep_short_names(out):
    """Table names vtspan --sweep flagged short, from its own output."""
    names = set()
    for ln in out.splitlines():
        m = re.match(r"^(\S+)\s+.*<==\s*SHORT BY", ln)
        if m:
            names.add(m.group(1))
    return names


def fills_short_names(out):
    """Fill function names vtspan --fills flagged, from its own output.

    Two shapes in that report and both count:
        _ZTV3MrI (hal_fill_mri_vtable)          leaves a slot unwritten
        hal_fill_seaweed_vtable   _ZTV... is a host-only name   unresolved
    The second is a GAP in the check rather than a defect, but an unresolved
    fill that nobody listed is still something nobody looked at, so it is
    ratcheted the same way.
    """
    names = set()
    for ln in out.splitlines():
        m = re.match(r"^\S+\s+\((\S+)\)\s*$", ln.strip())
        if m:
            names.add(m.group(1))
            continue
        m = re.match(r"^\s+(hal_\S+|port_\S+)\s+\S+\s+is a host-only name",
                     ln)
        if m:
            names.add(m.group(1))
    return names


def run(label, argv, root, quiet=False):
    print("\n" + "=" * 72)
    print("## %s" % label)
    print("   %s" % " ".join(argv))
    print("=" * 72, flush=True)
    p = subprocess.run(argv, cwd=root, capture_output=True, text=True)
    if quiet:
        # A selftest that passes says so in one line; print only that.
        tail = [ln for ln in p.stdout.splitlines() if ln.strip()][-1:]
        print("\n".join(tail))
    else:
        sys.stdout.write(p.stdout)
    if p.stderr:
        sys.stderr.write(p.stderr)
    sys.stdout.flush()
    return p.returncode, p.stdout


def main(argv):
    ap = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("root", nargs="?",
                    default=os.path.dirname(os.path.dirname(HERE)))
    ap.add_argument("--abicheck", action="store_true",
                    help="also run abicheck (needs --disasm-dir)")
    ap.add_argument("--disasm-dir", metavar="DIR",
                    help="dumpbin /disasm:nobytes output for the hal objects; "
                         "implies --abicheck")
    ap.add_argument("--skip-selftests", action="store_true",
                    help="do not run stage 0. The run then reports "
                         "INCOMPLETE, never a clean pass.")
    args = ap.parse_args(argv[1:])

    root = os.path.abspath(args.root)
    want_abicheck = args.abicheck or bool(args.disasm_dir)
    failures = []

    # ---- stage 0: the checkers' own fixtures --------------------------------
    if args.skip_selftests:
        print("stage 0 SKIPPED: the checkers' fixtures were not run, so a "
              "checker that stopped matching its shape would report this tree "
              "clean. This run cannot be a clean pass.")
        failures.append("stage 0 skipped (fixtures unverified)")
    else:
        for tool in SELFTESTS:
            rc, _ = run("selftest: %s" % tool,
                        [PY, os.path.join(HERE, tool), "--selftest"], root,
                        quiet=True)
            if rc != 0:
                failures.append("%s --selftest exit %d" % (tool, rc))
        if failures:
            print("\n" + "=" * 72)
            print("REFUSED before checking the tree: a checker's own fixtures "
                  "failed.")
            for f in failures:
                print("  - %s" % f)
            print("=" * 72)
            return 1

    # ---- stage 1: no-build checkers ----------------------------------------
    rc, _ = run("aliascheck (no build)",
                [PY, os.path.join(HERE, "aliascheck.py"), root], root)
    if rc != 0:
        failures.append("aliascheck exit %d" % rc)

    rc, out = run("vtspan --sweep (no build)",
                  [PY, os.path.join(HERE, "vtspan.py"), root, "--sweep"], root)
    # vtspan --sweep exits 1 if ANY host vtable is short. Gate against a
    # committed baseline: a short table NOT in the baseline is a NEW
    # regression and fails; the baselined ones are tolerated and the list may
    # only shrink.
    allowed = load_baseline(SWEEP_BASELINE)
    new_short = sweep_short_names(out) - allowed
    if new_short:
        print("\nvtspan --sweep: NEW short table(s) not in the baseline: %s"
              % ", ".join(sorted(new_short)))
        failures.append("vtspan --sweep: new short table(s) %s"
                        % ", ".join(sorted(new_short)))
    elif rc != 0:
        still = sweep_short_names(out) & allowed
        print("\nvtspan --sweep: %d baselined short table(s) remain (%s); no "
              "new regression." % (len(still), ", ".join(sorted(still))))

    rc, out = run("vtspan --fills (no build)",
                  [PY, os.path.join(HERE, "vtspan.py"), root, "--fills"], root)
    # Same ratchet as --sweep, for the same reason: five fills leave a slot
    # unwritten on cons and two cannot be resolved, all of it pre-existing. A
    # gate that is red the day it lands is a gate somebody switches off.
    allowed = load_baseline(FILLS_BASELINE)
    live = fills_short_names(out)
    new_fills = live - allowed
    if new_fills:
        print("\nvtspan --fills: NEW short fill(s) not in the baseline: %s"
              % ", ".join(sorted(new_fills)))
        failures.append("vtspan --fills: new short fill(s) %s"
                        % ", ".join(sorted(new_fills)))
    elif rc != 0:
        print("\nvtspan --fills: %d baselined fill(s) remain (%s); no new "
              "regression. They are DEBT, not clearance."
              % (len(live & allowed), ", ".join(sorted(live & allowed))))
    retired = allowed - live
    if retired:
        print("vtspan --fills: baselined fill(s) no longer flagged, delete "
              "the rows: %s" % ", ".join(sorted(retired)))

    # aritycheck: the full census never fails (its parser is a regex over C
    # and C++ text and it reports two thousand rows), but TWO subsets are
    # ratcheted and do gate. The RECEIVER SHAPE is exactly what PR #1539 and
    # PR #1543 fixed on 2026-08-16; the PLAIN-NAME shape is exactly what
    # shipped the Sort or 'Splode bin-full softlock that ba2224f88 fixed on
    # 2026-08-22 (run mg8, lane SBN), which the receiver scoping could not
    # see. One invocation runs both; the output names whichever failed.
    rc, _ = run("aritycheck --gate-receiver --gate-plainfunc (no build; "
                "full census is report-only, the two subsets are ratchets)",
                [PY, os.path.join(HERE, "aritycheck.py"), root,
                 "--gate-receiver", "--gate-plainfunc"], root)
    if rc != 0:
        failures.append("aritycheck --gate-receiver/--gate-plainfunc "
                        "exit %d" % rc)

    # ---- stage 2: the build-requiring checker ------------------------------
    if want_abicheck:
        disasm = args.disasm_dir
        if not disasm:
            print("\nabicheck: --abicheck was given without --disasm-dir. "
                  "Generate the input with port/tools/gen_disasm.cmd "
                  "<build-dir> <out-dir> and pass that directory.")
            failures.append("abicheck: no --disasm-dir")
        elif not os.path.isdir(disasm):
            print("\nabicheck: disasm directory %s does not exist" % disasm)
            failures.append("abicheck: no disasm directory")
        elif not any(f.endswith(".txt") for f in os.listdir(disasm)):
            print("\nabicheck: %s holds no .txt disassembly -- did the build "
                  "and the dumpbin step run?" % disasm)
            failures.append("abicheck: empty disasm directory")
        else:
            rc, out = run("abicheck (build + dumpbin /disasm:nobytes)",
                          [PY, os.path.join(HERE, "abicheck.py"),
                           "--disasm-dir", disasm], root)
            m = re.search(r"(\d+) vtable slot fills", out)
            fills = int(m.group(1)) if m else 0
            if rc != 0:
                failures.append("abicheck exit %d" % rc)
            elif fills == 0:
                print("\nabicheck parsed 0 vtable slot fills -- VACUOUS, "
                      "treated as failure.")
                failures.append("abicheck: 0 vtable slot fills (vacuous)")
            else:
                print("\nabicheck read %d vtable slot fills." % fills)
    else:
        print("\n(abicheck skipped: build the port, run "
              "port/tools/gen_disasm.cmd <build-dir> <out-dir>, then pass "
              "--disasm-dir <out-dir> to run it)")

    # ---- verdict ------------------------------------------------------------
    print("\n" + "=" * 72)
    if failures:
        print("PORT ABI CHECKS FAILED:")
        for f in failures:
            print("  - %s" % f)
        print("=" * 72)
        return 1
    if not want_abicheck:
        print("PORT ABI CHECKS PASSED (no-build set). abicheck did NOT run, "
              "so the emitted-code half of the class is unchecked by this "
              "run.")
    else:
        print("PORT ABI CHECKS PASSED (every checker ran and checked "
              "something)")
    print("=" * 72)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
