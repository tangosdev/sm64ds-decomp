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

Files carrying a `// NONMATCHING` banner are drafts by definition and are skipped.

Usage:
  python tools/prepush_linkcheck.py --range origin/main..HEAD   # changed src + header consumers
  python tools/prepush_linkcheck.py --files src/a.c src/b.cpp
  python tools/prepush_linkcheck.py --range origin/main..HEAD --json report.json

Install as a git hook (local, not committed):
  see tools/hooks/pre-push
"""
import argparse
import json
import pathlib
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
sys.path.insert(0, str(REPO / "tools"))
import asm_policy  # noqa: E402
import affected_src as A  # noqa: E402

# Fail closed. WRONG/NO-REPRO are false matches; ERROR means we could not get a verdict at
# all, and a gate that waves through what it could not check is not a gate. BLIND and NO-SYM
# stay warnings: those are symbol-coverage gaps, not evidence the bytes are wrong.
BLOCKING = {"WRONG", "NO-REPRO", "ERROR"}


def _load_symbol(name):
    """(module, addr, size) for a symbol, via the resolver stamp_provenance already uses."""
    import importlib.util

    spec = importlib.util.spec_from_file_location("_sp", REPO / "tools" / "stamp_provenance.py")
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod.load_symbol(name)


def changed_src_files(rev_range):
    out = subprocess.run(
        ["git", "diff", "--name-only", "--diff-filter=d", rev_range,
         "--", "src/", "include/"],
        cwd=REPO, capture_output=True, text=True,
    )
    if out.returncode != 0:
        return []
    changed = [f.strip() for f in out.stdout.splitlines() if f.strip()]
    expanded = A.affected_sources(changed)
    # Say so when headers pulled in files the diff never named. A silent expansion from 2
    # files to 300 reads as a hung gate; a silent expansion to 0 reads as "checked, clean".
    headers = [f for f in changed if f.endswith((".h", ".hpp"))]
    if headers:
        print(f"{len(headers)} changed header(s) fan out to {len(expanded)} source file(s) "
              f"to verify", flush=True)
    return expanded


def is_draft(path):
    p = REPO / path
    if not p.exists():
        return False
    return asm_policy.has_draft_banner(p.read_text(errors="ignore"))


def _run_linkcheck(module, name, addr, size):
    """One linkcheck invocation -> (verdict, blind, diffs). verdict 'ERROR' if no JSON came back."""
    proc = subprocess.run(
        [sys.executable, str(REPO / "tools" / "linkcheck.py"),
         "--module", module, "--name", name,
         "--addr", hex(addr), "--size", hex(size)],
        cwd=REPO, capture_output=True, text=True,
    )
    try:
        # Decode from the FIRST '{'. Taking the last one lands inside the "diffs" array on
        # exactly the runs that matter - a WRONG verdict carries diff objects, so rfind hit a
        # fragment, json.loads raised, and the verdict silently degraded to ERROR. ERROR was
        # then only a warning, so the gate waved through the false matches it exists to stop.
        start = proc.stdout.find("{")
        if start != -1:
            rec, _ = json.JSONDecoder().raw_decode(proc.stdout[start:])
            return rec.get("verdict", "ERROR"), rec.get("blind", 0), rec.get("diffs", [])
    except (ValueError, TypeError):
        pass
    return "ERROR", 0, []


def verify(path):
    """Run linkcheck for one src file. Returns a result dict.

    ERROR means linkcheck produced no parseable verdict - a compile timeout or a transient
    reverify import failure, which happens once in a while when a whole batch runs back to
    back. It is not evidence the file is bad, so retry once before treating it as blocking;
    a genuinely broken file (WRONG / NO-REPRO) returns a real verdict on the first try."""
    name = pathlib.Path(path).stem
    sym = _load_symbol(name)
    if not sym:
        return {"file": path, "name": name, "verdict": "NO-SYM", "note": "no symbol entry"}
    module, addr, size = sym
    verdict, blind, diffs = _run_linkcheck(module, name, addr, size)
    if verdict == "ERROR":
        verdict, blind, diffs = _run_linkcheck(module, name, addr, size)  # one retry on a flaky run
    return {"file": path, "name": name, "module": module, "addr": hex(addr),
            "verdict": verdict, "blind": blind, "diffs": diffs}


def main():
    ap = argparse.ArgumentParser(description="Link-verify matched src/ files before a push")
    ap.add_argument("--range", default="origin/main..HEAD", help="git rev range to inspect")
    ap.add_argument("--files", nargs="*", help="explicit file list (overrides --range)")
    ap.add_argument("--json", help="write a JSON report here")
    args = ap.parse_args()

    files = args.files if args.files else changed_src_files(args.range)
    if not files:
        print("prepush-linkcheck: no matched src/ files in this push - nothing to verify")
        return 0

    results, blocked, warned, drafts = [], [], [], []
    for f in files:
        if is_draft(f):
            drafts.append(f)
            continue
        r = verify(f)
        results.append(r)
        if r["verdict"] in BLOCKING:
            blocked.append(r)
        elif r["verdict"] != "VERIFIED":
            warned.append(r)
        icon = "OK  " if r["verdict"] == "VERIFIED" else "FAIL" if r["verdict"] in BLOCKING else "WARN"
        print(f"  [{icon}] {r['name']:<44} {r['verdict']}")

    if drafts:
        print(f"prepush-linkcheck: skipped {len(drafts)} NONMATCHING draft(s)")
    if args.json:
        pathlib.Path(args.json).write_text(json.dumps(results, indent=1))

    ok = sum(1 for r in results if r["verdict"] == "VERIFIED")
    print(f"prepush-linkcheck: {len(results)} checked - "
          f"{ok} verified, {len(warned)} warning(s), {len(blocked)} blocking")
    if blocked:
        print("\nPUSH BLOCKED - these do not reproduce the ROM:", file=sys.stderr)
        for r in blocked:
            print(f"  {r['name']} ({r['verdict']}) - {r['file']}", file=sys.stderr)
        print("\nA non-reproducing file in src/ is a false match someone has to rip back out.",
              file=sys.stderr)
        print("Move it to nearmiss/db.jsonl (tools/nearmiss_db.py) or fix it, then push again.",
              file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
