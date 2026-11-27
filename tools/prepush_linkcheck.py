"""Pre-push gate: link-verify every matched src/ file a push would publish.

WHY THIS EXISTS
---------------
The `validate` CI check runs on `pull_request_target` only (.github/workflows/pr-validate.yml),
because the real compile+linkcheck needs the ROM and happens on a private build box reached
through a relay keyed by PR number. Anything pushed straight to main therefore lands with NO
byte-match gate at all. This closes that hole locally: the box you push from already has the
ROM and the toolchain, so run the same link verification here, before the push leaves.

Verdicts (from tools/linkcheck.py) and how they are treated:
  VERIFIED   linked bytes equal the ROM                      -> pass
  BLIND-n    n reloc slots unresolvable, rest verified       -> warn (symbol coverage, not wrongness)
  WRONG      a resolved reloc links to bytes that differ     -> BLOCK (false match)
  NO-REPRO   compiles to the right length, wrong bytes       -> BLOCK (false match)
  NO-SYM / NO-BIN / other                                    -> warn (environment/coverage)
  NO-COMPILE the TU does not compile at all                    -> BLOCK (nothing else ran)

NO-BIN IS WHY THE COMPILE CHECK EXISTS
--------------------------------------
linkcheck needs --bin/--base to place an overlay, so for an overlay source EVERY function
comes back NO-BIN -- a coverage warning, correctly, because no bytes were compared. But a
warning on every row means an overlay TU that does not COMPILE also reports nothing
blocking: PR #4553 added a declaration to include/math/CalcLib.h that contradicted the
local `Vec3_Asr(void *, void *, int)` in seven overlay actors, every one of them stopped
compiling, and this gate said `0 blocking` while the validator failed the full ROM build.
So each file is compiled once up front. A compile failure needs no ROM, no overlay base
and no symbol table, it is never flaky, and nothing downstream of it can be trusted --
hence BLOCK, for overlay and arm9 sources alike.

Files carrying a `// NONMATCHING` banner are drafts by definition and are skipped.

RESOLVING A FILE TO THE FUNCTION(S) IT VERIFIES
------------------------------------------------
A legacy source owns exactly one function, named by its filename -- `src/<name>.c`. A
consolidated translation unit (a merged multi-function `.cpp`, or a hand-authored actor
file whose name is not any symbol it defines) owns however many functions its committed
`config/**/delinks.txt` address range covers. `srcpath.symbols_for` answers both cases
from the same table dsd itself reads to link the ROM, so a file that owns more than one
function gets one verdict PER FUNCTION here -- the file cannot be waved through as
NO-SYM just because its own name matches nothing. An unenrolled file still falls back to
its filename, so nothing here changes for a source with no delinks.txt entry yet.

WHERE THE TIME GOES, AND WHY IT IS IN-PROCESS NOW
---------------------------------------------------
This used to spawn `tools/linkcheck.py` once PER FUNCTION. Each of those processes
paid interpreter and import start-up, rebuilt the symbol-name index, and -- the real
cost -- recompiled the function's whole translation unit from scratch, because nothing
outlives a process. A consolidated TU with 140 functions was compiled 140+ times. On a
shared-header change that is ruinous: PR #4871's `decl_common.h` edit fans out to 129
files / 3,644 functions, which ran at ~2 s a function, about two hours, and agents'
30-minute caps killed it before it reached the TU the PR actually changed.

So the same `linkcheck.linkcheck()` call now runs in-process, with one name index per
process and `match.memoize_compiles()` on (this gate checks one tree that nothing edits
underneath it, which is exactly the condition that memo needs), and the files are
spread across a process pool: the per-function work after the compile is pure-Python
ELF parsing, which threads would serialise on the GIL. WHAT is checked did not change.
Every function still gets the identical `linkcheck(name, addr, size, module,
name_index)` call the subprocess made, with the same one retry on ERROR.

The files the diff names directly run first, and a header's fan-out runs after them.
With `--time-budget`, a run that cannot finish stops at the budget, says exactly which
files it did and did not reach, and exits 2: an incomplete run is never a pass.

Usage:
  python tools/prepush_linkcheck.py --range origin/main..HEAD   # changed src + header consumers
  python tools/prepush_linkcheck.py --files src/a.c src/b.cpp
  python tools/prepush_linkcheck.py --range origin/main..HEAD --json report.json
  python tools/prepush_linkcheck.py --range origin/main..HEAD -j 16 --time-budget 1500

Install as a git hook (local, not committed):
  see tools/hooks/pre-push
"""
import argparse
import json
import pathlib
import subprocess
import sys
import tempfile
import time

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import asm_policy  # noqa: E402
import affected_src as A  # noqa: E402
import srcpath  # noqa: E402

# Fail closed. WRONG/NO-REPRO are false matches; ERROR means we could not get a verdict at
# all, and a gate that waves through what it could not check is not a gate. BLIND and NO-SYM
# stay warnings: those are symbol-coverage gaps, not evidence the bytes are wrong.
BLOCKING = {"WRONG", "NO-REPRO", "ERROR", "NO-COMPILE"}

# The pinned compiler the whole tree is matched against.
VERSION = "2004/b56"


_SYMBOLS = None


def _symbol_index():
    """name -> (module, addr, size) for every function row, built ONCE per process.

    The same walk as stamp_provenance.load_symbol: relocs.module_universe's files in its
    order, stamp_provenance.FUNC_RE on each line, first hit wins. load_symbol re-reads
    every symbols.txt on each lookup, and this gate used to re-execute the whole
    stamp_provenance module on each lookup too."""
    global _SYMBOLS
    if _SYMBOLS is None:
        import importlib.util
        import relocs

        spec = importlib.util.spec_from_file_location("_sp", REPO / "tools" / "stamp_provenance.py")
        sp = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(sp)
        index = {}
        for path, module in relocs.module_universe(repo=sp.get_repo()):
            for line in path.read_text(errors="ignore").splitlines():
                m = sp.FUNC_RE.match(line)
                if m:
                    index.setdefault(m.group(1), (module, int(m.group(3), 16),
                                                  int(m.group(2), 16)))
        _SYMBOLS = index
    return _SYMBOLS


def _load_symbol(name):
    """(module, addr, size) for a symbol, or None: stamp_provenance.load_symbol's answer."""
    return _symbol_index().get(name)


def changed_src_files(rev_range):
    """Every source the range affects, with the files the diff names directly first.

    The order is the point. A shared-header edit fans out to a hundred-odd consumers, and
    a run that is cut short must already have checked the files the change is about."""
    out = subprocess.run(
        ["git", "diff", "--name-only", "--diff-filter=d", rev_range,
         "--", "src/", "include/"],
        cwd=REPO, capture_output=True, text=True,
    )
    if out.returncode != 0:
        return []
    changed = [f.strip() for f in out.stdout.splitlines() if f.strip()]
    expanded = A.affected_sources(changed)
    named = set(changed)
    direct = [f for f in expanded if f in named]
    # Largest TUs first within the fan-out, so the pool does not finish on one 150-function
    # straggler while every other worker sits idle.
    fanout = sorted((f for f in expanded if f not in named),
                    key=lambda f: -len(srcpath.symbols_for(f)))
    # Say so when headers pulled in files the diff never named. A silent expansion from 2
    # files to 300 reads as a hung gate; a silent expansion to 0 reads as "checked, clean".
    headers = [f for f in changed if f.endswith((".h", ".hpp"))]
    if headers:
        print(f"{len(headers)} changed header(s) fan out to {len(expanded)} source file(s) "
              f"to verify ({len(direct)} named by the diff, checked first)", flush=True)
    return direct + fanout


def is_draft(path):
    p = REPO / path
    if not p.exists():
        return False
    return asm_policy.has_draft_banner(p.read_text(errors="ignore"))


_NAME_INDEX = None


def _linkcheck_setup():
    """Per-process state for `_run_linkcheck`: the reloc-target name index, and compile
    memoisation. The memo is safe here and not in tools that edit headers between
    compiles (see match.memoize_compiles), because this gate checks one tree that nothing
    edits underneath it. Idempotent: the serial path and every pool worker call it."""
    global _NAME_INDEX
    if _NAME_INDEX is None:
        import match
        import reloc_audit as RA

        match.memoize_compiles()
        _NAME_INDEX = RA.build_name_index()
    return _NAME_INDEX


def _run_linkcheck(module, name, addr, size):
    """One linkcheck -> (verdict, blind, diffs). verdict 'ERROR' if it raised.

    In-process, but the same call `tools/linkcheck.py --module --name --addr --size`
    makes: `linkcheck.linkcheck(name, addr, size, module, build_name_index())`, with no
    candidate override. An exception is what used to surface as "no JSON on stdout", so
    it keeps that ERROR verdict and the caller's one retry."""
    import contextlib
    import io

    try:
        import linkcheck as LC

        # The subprocess's stdout was captured and only its JSON read; the compiler's
        # "! compile failed" chatter must not land in this gate's report either.
        with contextlib.redirect_stdout(io.StringIO()):
            rec = LC.linkcheck(name, addr, size, module, _linkcheck_setup())
        return rec.get("verdict", "ERROR"), rec.get("blind", 0), rec.get("diffs", [])
    except Exception:  # noqa: BLE001 -- a crash is an ERROR verdict, never a pass
        return "ERROR", 0, []


def _compiles(path):
    """(ok, first_diagnostic) for `path` under the pinned compiler.

    Separate from linkcheck on purpose: this answers "is this TU even valid C++ against
    the headers in the working tree", which is the question a header edit actually
    raises, and it is the only check here that an overlay source cannot dodge.
    mwccarm is noisy about MWCIncludes and union members, so only lines that are not
    warnings are reported back."""
    import match

    # A MISSING toolchain must not read as a broken file: match.compile_c returns None for
    # both, and a gate that blocks every push on a box without mwccarm is worse than one
    # that skips this check there.
    if not (match.MW / VERSION / "mwccarm.exe").is_file():
        return True, ""

    src = REPO / path
    # A path the diff names but the tree no longer has (a delete, or the old side of a
    # rename) is not a compile failure; the symbol loop reports it as NO-SYM instead.
    if not src.is_file():
        return True, ""
    with tempfile.TemporaryDirectory() as d:
        staged = pathlib.Path(d) / src.name  # the basename is load-bearing: __sinit_<file>
        staged.write_bytes(src.read_bytes())
        flags = match.resolve_cpp_flags(src, match.DEFAULT_FLAGS)
        obj = match.compile_c(staged, VERSION, flags)
    if obj:
        return True, ""
    return False, "did not compile at %s" % VERSION


def verify(path):
    """Run linkcheck for every function `path` owns. Returns a list of result dicts, one
    row per function -- a legacy one-function source still returns a list of exactly one,
    so every caller that used to hold a single dict now holds `verify(path)[0]` for that
    case and a longer list for a consolidated TU.

    `srcpath.symbols_for` is the resolver: the enrolment table (config/**/delinks.txt's
    address range for this file, matched against config/**/symbols.txt) first, the
    filename stem as a fallback for anything not enrolled there yet. That table is what
    dsd itself reads to link the ROM, so a file that owns several functions is asked for
    all of them instead of being reduced to the one name its own filename happens to
    spell -- the gap this tool used to fall into for every merged TU and every
    convention-named actor file.

    ERROR means linkcheck produced no parseable verdict - a compile timeout or a transient
    reverify import failure, which happens once in a while when a whole batch runs back to
    back. It is not evidence the file is bad, so retry once before treating it as blocking;
    a genuinely broken file (WRONG / NO-REPRO) returns a real verdict on the first try."""
    ok, why = _compiles(path)
    if not ok:
        return [{"file": path, "name": pathlib.Path(path).name,
                 "verdict": "NO-COMPILE", "note": why}]

    rows = []
    for name in srcpath.symbols_for(path):
        sym = _load_symbol(name)
        if not sym:
            rows.append({"file": path, "name": name, "verdict": "NO-SYM",
                         "note": "no symbol entry"})
            continue
        module, addr, size = sym
        verdict, blind, diffs = _run_linkcheck(module, name, addr, size)
        if verdict == "ERROR":
            verdict, blind, diffs = _run_linkcheck(module, name, addr, size)  # one retry on a flaky run
        rows.append({"file": path, "name": name, "module": module, "addr": hex(addr),
                     "verdict": verdict, "blind": blind, "diffs": diffs})
    return rows


def _verify_or_draft(path):
    """verify(path), or None for a NONMATCHING draft. The unit of work a pool worker runs.

    An exception is an ERROR row, which blocks: a file this gate could not check must not
    read as a file that passed, and one bad file must not take the other results with it."""
    try:
        if is_draft(path):
            return None
        return verify(path)
    except Exception as exc:  # noqa: BLE001
        return [{"file": path, "name": pathlib.Path(path).name, "verdict": "ERROR",
                 "note": f"{type(exc).__name__}: {exc}"}]


def _worker_init():
    sys.path.insert(0, str(REPO / "tools"))
    _linkcheck_setup()
    _symbol_index()


def _default_jobs():
    import os
    return max(1, os.cpu_count() or 1)


def _results_in_order(files, jobs, deadline):
    """Yield (path, rows-or-None) in `files` order, stopping at `deadline` (monotonic).

    The files are independent and the per-function work is CPU-bound Python, so they go
    to a process pool; each worker keeps its own compile memo and indexes. Results are
    still yielded, printed and reported in `files` order, so a -j16 log reads like -j1.
    At the deadline the files already finished are yielded (out of order, when a longer
    one ahead of them is still running), the pool is terminated, and the caller reports
    what was not reached. The serial path checks the deadline between files."""
    if jobs <= 1 or len(files) <= 1:
        for f in files:
            if deadline is not None and time.monotonic() >= deadline:
                return
            yield f, _verify_or_draft(f)
        return
    import multiprocessing as mp

    pool = mp.get_context("spawn").Pool(min(jobs, len(files)), initializer=_worker_init)
    try:
        pending = [(f, pool.apply_async(_verify_or_draft, (f,))) for f in files]
        for i, (f, res) in enumerate(pending):
            timeout = None if deadline is None else max(0.0, deadline - time.monotonic())
            try:
                rows = res.get(timeout)
            except mp.TimeoutError:
                # Out of time. Whatever the workers already FINISHED is still a real
                # result -- one long file early in the order must not discard the short
                # ones behind it -- so hand those over before stopping.
                for g, later in pending[i:]:
                    if later.ready():
                        yield g, later.get()
                return
            yield f, rows
    finally:
        pool.terminate()
        pool.join()


def main():
    ap = argparse.ArgumentParser(description="Link-verify matched src/ files before a push")
    ap.add_argument("--range", default="origin/main..HEAD", help="git rev range to inspect")
    ap.add_argument("--files", nargs="*", help="explicit file list (overrides --range)")
    ap.add_argument("--json", help="write a JSON report here")
    ap.add_argument("-j", "--jobs", type=int, default=_default_jobs(),
                    help="files to check at once (default: CPU count)")
    ap.add_argument("--time-budget", type=float, default=None, metavar="SECONDS",
                    help="stop after this long, report the coverage reached, and exit 2 "
                         "(an incomplete run is not a pass). Set it below any outer "
                         "timeout so the run reports instead of being killed.")
    args = ap.parse_args()

    started = time.monotonic()
    deadline = None if args.time_budget is None else started + args.time_budget
    files = args.files if args.files else changed_src_files(args.range)
    if not files:
        print("prepush-linkcheck: no matched src/ files in this push - nothing to verify")
        return 0

    results, blocked, warned, drafts, done = [], [], [], [], []
    for f, rows in _results_in_order(files, args.jobs, deadline):
        done.append(f)
        if rows is None:
            drafts.append(f)
            continue
        if len(rows) > 1:
            print(f"  {f}  ({len(rows)} functions)")
        for r in rows:
            results.append(r)
            if r["verdict"] in BLOCKING:
                blocked.append(r)
            elif r["verdict"] != "VERIFIED":
                warned.append(r)
            icon = "OK  " if r["verdict"] == "VERIFIED" else "FAIL" if r["verdict"] in BLOCKING else "WARN"
            name = f"  {r['name']}" if len(rows) > 1 else r["name"]
            print(f"  [{icon}] {name:<44} {r['verdict']}", flush=True)

    reached = set(done)
    unchecked = [f for f in files if f not in reached]
    if drafts:
        print(f"prepush-linkcheck: skipped {len(drafts)} NONMATCHING draft(s)")
    if args.json:
        report = results + [{"file": f, "verdict": "UNCHECKED",
                             "note": "time budget reached first"} for f in unchecked]
        pathlib.Path(args.json).write_text(json.dumps(report, indent=1))

    ok = sum(1 for r in results if r["verdict"] == "VERIFIED")
    elapsed = time.monotonic() - started
    print(f"prepush-linkcheck: {len(results)} checked - "
          f"{ok} verified, {len(warned)} warning(s), {len(blocked)} blocking "
          f"({len(done)}/{len(files)} files, {elapsed:.0f}s, -j{args.jobs})")
    if unchecked:
        print(f"\nprepush-linkcheck: INCOMPLETE - the {args.time_budget:g}s time budget ran "
              f"out with {len(unchecked)} of {len(files)} file(s) not checked:",
              file=sys.stderr)
        for f in unchecked:
            print(f"  {f}", file=sys.stderr)
        print("Nothing above vouches for those files. Re-run with a larger budget, or "
              "check them with --files.", file=sys.stderr)
    if blocked:
        print("\nPUSH BLOCKED - these do not reproduce the ROM:", file=sys.stderr)
        for r in blocked:
            print(f"  {r['name']} ({r['verdict']}) - {r['file']}", file=sys.stderr)
        print("\nA non-reproducing file in src/ is a false match someone has to rip back out.",
              file=sys.stderr)
        print("Move it to nearmiss/db.jsonl (tools/nearmiss_db.py) or fix it, then push again.",
              file=sys.stderr)
        return 1
    return 2 if unchecked else 0


if __name__ == "__main__":
    sys.exit(main())
