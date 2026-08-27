"""Every src_tu/ translation unit must still COMPILE under the pin the build would use.

WHY THIS EXISTS
---------------
`tools/check_src_tu.py` (#1667) closed half a hole: it proves every `#include` resolves
and every mangled symbol a merged TU names is a symbol the ROM configuration knows. Its
own "WHAT IT DOES NOT CHECK" section names the other half:

    Types, signatures and layout. A TU can pass this gate and still not compile,
    because a header can retype a field or change a virtual's return type without
    renaming anything.

That is not hypothetical, it is what happened. #1583 retyped three `dActor_c` virtuals
from `int` to `void` and swept only `src/`. The then-shadow TU now promoted at
`src/game/actors/Actor.cpp` still declared them
`int`, so it stopped compiling -- and NOTHING WENT RED, for the same reason #1643's
rename went unnoticed: an unbuildable source file is not a failing file, it is an ABSENT
one. It produces no object, so it produces no mismatch, and `rombuild.py` went on
reporting 106/106 exact with the file simply gone from coverage (notes:
unbuildable-files-invisible). A resolution check cannot see a return type. Only the
compiler can, so this gate runs the compiler.

WHERE IT RUNS
-------------
Not on a GitHub runner. mwccarm and its licence are not there -- `src-tu-refs.yml` says
so in its own header, and `pr-validate.yml` exists precisely because the compiler-and-ROM
half of validation happens on the private build box behind the relay. This gate belongs
where `tools/hooks/pre-push` already puts the compiler-dependent checks ("this box has
the ROM + toolchain") and where the validator worker runs. It needs the compiler and
`config/rombuild-versions.txt`; it does NOT need the extracted ROM, because it compiles
and never compares bytes -- which is what makes it cheap enough to run on every push
(~0.06s per TU, about three seconds for all 41).

A MISSING COMPILER IS A FAILURE, NOT A SKIP
-------------------------------------------
`tools/test_build_pin.py` and `tools/test_tubuild.py` both gate their end-to-end cases on
a `_toolchain()` predicate and `return` when it is false, so an unwired worktree runs them
and reports green having compiled nothing. That is the same false-green shape this whole
family of gates exists to close, arrived at from inside the test suite. So there is no
skip here and no flag that turns one on: if the pinned compiler is not installed this
exits non-zero and says which compiler and which path. A caller that cannot provide a
compiler must not run this gate rather than run it and read the result as a pass.

WHAT IT CHECKS
--------------
1. toolchain - the pinned mwccarm for each TU is installed, and so is its licence file.
2. coverage  - the manifest and the src_tu/ tree agree on the work list. A source on
               disk with no manifest entry has no knowable pin and is not compiled by
               anything; a manifest entry with no source on disk is a stranded record.
               Either way the count this gate reports would otherwise be quietly wrong.
3. compile   - the TU compiles, with the version `tubuild` would pick (derived from each
               member function's legacy pin) and the flags `build_pin.flags_for` gives,
               which are `rombuild.CFLAGS` -- the build's own, not `match.DEFAULT_FLAGS`.
4. worklist  - it found translation units at all. An empty check is not a pass.

Usage:
  python tools/check_src_tu_compiles.py                 # compile every src_tu TU
  python tools/check_src_tu_compiles.py --json          # machine-readable, to stdout
  python tools/check_src_tu_compiles.py --json out.json
  python tools/check_src_tu_compiles.py --id ov045/PoleLift    # one TU, repeatable
  python tools/check_src_tu_compiles.py --quiet         # only the failures

Exit code 0 when every translation unit compiles, 1 otherwise -- including a missing
compiler, an inconsistent pin, a work list that does not match the tree, and an empty
work list.
"""
import argparse
import contextlib
import io
import json
import pathlib
import sys
import time

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))

import build_pin as BP          # noqa: E402
import match as M               # noqa: E402
import tu_manifest as TUM       # noqa: E402
import tubuild as TU            # noqa: E402

MANIFEST = TUM.DEFAULT_ROOT
SOURCE_SUFFIXES = (".c", ".cpp")


def _fail(check, path, message):
    return {"check": check, "file": path, "message": message}


def _sources_on_disk(root):
    return sorted(p.relative_to(REPO).as_posix()
                  for p in root.rglob("*") if p.suffix in SOURCE_SUFFIXES)


def compile_one(entry, version_override=None, include_dirs=()):
    """(ok, info). Never raises; every refusal is a reason.

    Deliberately calls `match.compile_c` -- the same function `tubuild._compile_tu` and
    the ROM build's own verifier reach -- rather than re-deriving an mwccarm command
    line here. A second spelling of the compile is a second thing to drift, and drift
    between the gate's compile and the build's compile is the defect `build_pin.py` was
    written to stop. `compile_c` narrates failures to stdout instead of returning them,
    so its output is captured for the report.

    `include_dirs` is `match.compile_c`'s own affordance -- "candidate-specific
    includes come first so an external workbench can be verified against its own
    headers". THE REAL RUN PASSES NONE, because `tubuild._compile_tu` passes none, so
    a src_tu file is searched against include/ and nothing else. It is here for
    fixtures. Note the asymmetry it makes visible: `check_src_tu.py` also accepts an
    include resolved against the source's OWN directory and mwccarm does not, so a
    src_tu file that used a sibling header would pass that gate and fail this one.
    That is this gate being right; do not "fix" it by adding the source directory.
    """
    src = REPO / entry["source"]
    if not src.is_file():
        return False, {"reason": f"source not found: {entry['source']}"}

    version, note = TU.resolve_tu_version(entry, version_override)
    if version is None:
        # Fails closed exactly like build_pin: an unknowable pin is not a default.
        return False, {"reason": f"no compiler version for this TU: {note}"}

    exe = M.MW / version / "mwccarm.exe"
    if not exe.is_file():
        return False, {"version": version, "toolchain": True,
                       "reason": f"pinned compiler {version} is not installed ({exe})"}

    flags = BP.flags_for(src)
    buf = io.StringIO()
    t0 = time.time()
    with contextlib.redirect_stdout(buf):
        obj = M.compile_c(src, version, flags, include_dirs)
    elapsed = time.time() - t0
    diag = buf.getvalue().strip()

    if obj is None:
        return False, {"version": version, "flags": flags, "seconds": round(elapsed, 3),
                       "reason": diag or "compile failed with no diagnostic"}
    return True, {"version": version, "flags": flags, "seconds": round(elapsed, 3),
                  "objectBytes": len(obj), "note": note, "diagnostic": diag or None}


def check(manifest_path=MANIFEST, root=None, only=(), version_override=None,
          on_result=None, include_dirs=()):
    root = pathlib.Path(root) if root else REPO / "src_tu"
    failures, results = [], []

    if not TUM.exists(manifest_path):
        return {"schemaVersion": 1, "ok": False,
                "checked": {"units": 0, "compiled": 0},
                "failed": 1,
                "failures": [_fail("worklist", manifest_path.as_posix(),
                                   "the TU manifest is missing -- there is no work list "
                                   "to check, which is not a pass")],
                "results": []}

    # The licence is as load-bearing as the executable and its absence produces a
    # compile error per file rather than one clear line, so say it once, up front.
    if not M.LICENSE.is_file():
        failures.append(_fail("toolchain", M.LICENSE.as_posix(),
                              "mwccarm licence file is missing -- every compile below "
                              "will fail for that reason and not for a source defect"))

    entries = TUM.load(manifest_path)["entries"]
    if only:
        want = set(only)
        unknown = sorted(want - {e["id"] for e in entries})
        for u in unknown:
            failures.append(_fail("worklist", manifest_path.as_posix(),
                                  f"--id {u} names no manifest entry"))
        entries = [e for e in entries if e["id"] in want]
    else:
        # Coverage, both directions. Only meaningful over the whole manifest.
        on_disk = set(_sources_on_disk(root))
        declared = {e["source"] for e in entries}
        for orphan in sorted(on_disk - declared):
            failures.append(_fail("coverage", orphan,
                                  "a translation unit on disk that the manifest does not "
                                  "declare -- nothing knows its compiler pin, so nothing "
                                  "compiles it and this gate cannot vouch for it"))
        # `on_disk` only walks `root`, so a TU that has been PROMOTED out of src_tu
        # into the real build tree (status "promoted", source under src/) is absent
        # from it while being perfectly present -- and it is still compiled below, so
        # the "one fewer TU checked" half of the claim is false too. Ask the disk.
        for stranded in sorted(declared - on_disk):
            if (REPO / stranded).is_file():
                continue
            failures.append(_fail("coverage", stranded,
                                  "the manifest declares this source and it is not on "
                                  "disk -- a stranded record, and one fewer TU checked "
                                  "than the count would suggest"))

    if not entries:
        failures.append(_fail("worklist", root.as_posix(),
                              "no translation units to compile -- an empty check is not "
                              "a pass; see the module docstring"))

    compiled = 0
    for entry in sorted(entries, key=lambda e: e["id"]):
        ok, info = compile_one(entry, version_override, include_dirs)
        row = {"id": entry["id"], "source": entry["source"], "ok": ok, **info}
        results.append(row)
        if ok:
            compiled += 1
        else:
            failures.append(_fail("toolchain" if info.get("toolchain") else "compile",
                                  entry["source"],
                                  f"{entry['id']}: {info['reason']}"))
        if on_result:
            on_result(row)

    return {
        "schemaVersion": 1,
        "ok": not failures,
        "checked": {"units": len(entries), "compiled": compiled,
                    "manifest": manifest_path.relative_to(REPO).as_posix(),
                    "root": root.relative_to(REPO).as_posix()},
        "failed": len(failures),
        "failures": failures,
        "results": results,
    }


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0],
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--root", default="src_tu",
                    help="directory of translation units (default: src_tu)")
    ap.add_argument("--manifest", default=None,
                    help="TU manifest (default: config/tu_manifest.d/)")
    ap.add_argument("--id", action="append", default=[], metavar="TU_ID",
                    help="compile only this TU; repeatable")
    ap.add_argument("--version", default=None,
                    help="force an mwccarm version instead of the pin (for measurement "
                         "only -- this is NOT what the build would use)")
    ap.add_argument("--json", nargs="?", const="-", metavar="PATH",
                    help="write the JSON report to PATH, or stdout with no PATH")
    ap.add_argument("--quiet", action="store_true", help="print only the failures")
    args = ap.parse_args(argv)

    root = REPO / args.root
    if not root.is_dir():
        print(f"check_src_tu_compiles: no such directory: {args.root}", file=sys.stderr)
        return 1
    manifest = pathlib.Path(args.manifest) if args.manifest else MANIFEST

    def narrate(row):
        if args.quiet and row["ok"]:
            return
        if row["ok"]:
            print(f"  ok    {row['id']:38} {row['version']:9} "
                  f"{row['objectBytes']:7d} bytes  {row['seconds']:.2f}s")
        else:
            print(f"  FAIL  {row['id']:38} {row.get('version', '-')}")

    started = time.time()
    report = check(manifest, root, tuple(args.id), args.version,
                   on_result=None if args.json == "-" else narrate)

    if args.json:
        text = json.dumps(report, indent=2) + "\n"
        if args.json == "-":
            sys.stdout.write(text)
        else:
            # build/ is gitignored, so a fresh checkout has no such directory --
            # writing the report must not be what fails the gate.
            out = pathlib.Path(args.json)
            out.parent.mkdir(parents=True, exist_ok=True)
            out.write_text(text)

    c = report["checked"]
    print(f"check_src_tu_compiles: {c['compiled']}/{c['units']} translation unit(s) "
          f"compiled in {time.time() - started:.1f}s")
    if report["ok"]:
        print("check_src_tu_compiles: every translation unit compiles.")
        return 0
    for f in report["failures"]:
        print(f"  {f['check']:9} {f['file']}: {f['message']}")
    print(f"check_src_tu_compiles: {report['failed']} failure(s).")
    return 1


if __name__ == "__main__":
    raise SystemExit(main())
