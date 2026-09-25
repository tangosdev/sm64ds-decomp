"""Every bannered NONMATCHING draft in src/ must still COMPILE under the build's pin.

WHY THIS EXISTS
---------------
A file whose header carries a NONMATCHING banner (`asm_policy.has_draft_banner`) is
left out of the ROM build: `tools/enroll.py` skips it, so the original bytes stand for
its range. The discipline in `tools/nonmatching.py` says a parked function "(a)
compiles". That is checked once, when it is parked, and never again, because no gate
compiles a file the build leaves out.

That is not hypothetical. On 2026-09-14 cd8fad9e04 landed
src/_ZN11dScMgJump_c13OnYoshiTryEatEi.cpp as a draft that passes the u32 counter
`unk_0bc` to func_ov006_020c44b4. The same day, 641d1c02d9 retyped that function's
first parameter in decl_common.h to `void *`. The draft stopped compiling and nothing
went red for nine days (#3085). It is the same false green `check_src_tu_compiles.py`
closes for src_tu/: an unbuildable file is an ABSENT one, not a failing one.

WHERE IT RUNS
-------------
Beside check_src_tu_compiles.py in tools/hooks/pre-push, for the same reasons: it
needs mwccarm and config/rombuild-versions.txt, not the ROM. It compiles and never
compares bytes, about 0.1s per draft.

A MISSING COMPILER IS A FAILURE, NOT A SKIP. An empty work list is a failure too.

Usage:
  python tools/check_drafts_compile.py            # compile every bannered draft
  python tools/check_drafts_compile.py --quiet    # only the failures
  python tools/check_drafts_compile.py src/func_ov006_020fc8c0.c   # named files only

Exit code 0 when every draft compiles, 1 otherwise.
"""
import argparse
import contextlib
import io
import pathlib
import sys
import time

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import asm_policy as AP         # noqa: E402
import build_pin as BP          # noqa: E402
import match as M               # noqa: E402

SOURCE_SUFFIXES = (".c", ".cpp")


def drafts(root=None):
    """Repo-relative paths of every src/ source that declares itself a non-match."""
    root = pathlib.Path(root) if root else REPO / "src"
    out = []
    for p in sorted(root.rglob("*")):
        if p.suffix not in SOURCE_SUFFIXES or not p.is_file():
            continue
        if AP.has_draft_banner(p.read_text(encoding="utf-8", errors="ignore")):
            out.append(p.relative_to(REPO).as_posix())
    return out


def compile_one(rel):
    """(ok, info). Never raises. Uses match.compile_c, the build's own compile path,
    with build_pin's version and flags, so the gate compiles what the build would."""
    src = REPO / rel
    if not src.is_file():
        return False, {"reason": f"source not found: {rel}"}
    version, why = BP.compiler_for(rel)
    if version is None:
        return False, {"toolchain": True, "reason": why}
    flags = BP.flags_for(src)
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        obj = M.compile_c(src, version, flags)
    diag = buf.getvalue().strip()
    if obj is None:
        return False, {"version": version,
                       "reason": diag or "compile failed with no diagnostic"}
    return True, {"version": version, "objectBytes": len(obj)}


def check(paths=None, on_result=None):
    failures, compiled = [], 0
    if not M.LICENSE.is_file():
        failures.append((M.LICENSE.as_posix(),
                         "mwccarm licence file is missing -- every compile below will "
                         "fail for that reason and not for a source defect"))
    work = list(paths) if paths else drafts()
    if not work:
        failures.append(("src", "no NONMATCHING drafts found -- an empty check is not "
                                "a pass"))
    for rel in work:
        ok, info = compile_one(rel)
        if ok:
            compiled += 1
        else:
            failures.append((rel, info["reason"]))
        if on_result:
            on_result(rel, ok, info)
    return {"ok": not failures, "drafts": len(work), "compiled": compiled,
            "failures": failures}


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0],
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("paths", nargs="*",
                    help="compile only these repo-relative files (default: every draft)")
    ap.add_argument("--quiet", action="store_true", help="print only the failures")
    args = ap.parse_args(argv)
    paths = [pathlib.Path(p).as_posix() for p in args.paths]

    def narrate(rel, ok, info):
        if ok and not args.quiet:
            print(f"  ok    {rel:60} {info['version']:9} {info['objectBytes']:7d} bytes")
        elif not ok:
            print(f"  FAIL  {rel:60} {info.get('version', '-')}")

    started = time.time()
    report = check(paths, narrate)
    print(f"check_drafts_compile: {report['compiled']}/{report['drafts']} draft(s) "
          f"compiled in {time.time() - started:.1f}s")
    if report["ok"]:
        print("check_drafts_compile: every NONMATCHING draft compiles.")
        return 0
    for rel, message in report["failures"]:
        print(f"  {rel}: {message}")
    print(f"check_drafts_compile: {len(report['failures'])} failure(s).")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
