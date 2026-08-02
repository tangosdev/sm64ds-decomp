#!/usr/bin/env python3
"""How much of src/ syntax-compiles for the host, and why the rest does not.

The port grows by pulling subsystems across the platform seam. This makes
that a measurement instead of a guess: every src/ file is syntax-checked
(cl /Zs -- no codegen, no linking) as the host would compile it, and each
failure is attributed to its FIRST error, bucketed by category. The report
answers "what single fix unblocks the most files next".

This deliberately measures the same thing Andrew's rombuild blocker report
measures for the NDS link, from the other side: there the question is "does
it still produce the ROM", here it is "does the host front-end accept it".

Usage:
  python port/tools/host_frontier.py                # summary table
  python port/tools/host_frontier.py --detail <bucket-substring>
  python port/tools/host_frontier.py --jobs 8 --batch 40

Requires the VS Build Tools 32-bit environment (run from build-port.cmd's
shell, or let the script locate vcvars itself, which it does by default).
"""
import argparse
import os
import re
import subprocess
import sys
import tempfile
from collections import Counter, defaultdict
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
SRC = REPO / "src"
INCLUDE = REPO / "include"
PORT = REPO / "port"

VCVARS = (r"C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools"
          r"\VC\Auxiliary\Build\vcvars32.bat")

# First-error attribution: MSVC error code -> human bucket. Anything not
# listed reports as its raw code so new classes surface instead of hiding
# in "other".
BUCKETS = {
    "C1083": "missing include",
    "C2065": "undeclared identifier",
    "C2061": "unknown type in declaration",
    "C2143": "syntax error (often a missing type)",
    "C4430": "missing type specifier",
    "C2059": "syntax error",
    "C2011": "type redefinition",
    "C2371": "redefinition: different basic types",
    "C2733": "extern-C overload",
    "C4235": "inline asm not supported on this arch",
    "C2467": "illegal declaration of anonymous struct",
    "C7626": "unnamed struct in typedef",
}


def vc_env():
    """Environment with the 32-bit MSVC toolchain on PATH, captured once.

    List-form cmd invocation: the shell=True string form breaks on this
    machine (cmd mis-parses the quoted path + redirect combination and
    reports 'cannot find the path specified' with empty output, which then
    surfaces later as FileNotFoundError on 'cl' -- a confusing distance from
    the actual failure)."""
    out = subprocess.run(
        ["cmd", "/c", "call", VCVARS, ">nul", "&&", "set"],
        capture_output=True, text=True, errors="replace")
    env = dict(os.environ)
    for line in out.stdout.splitlines():
        if "=" in line:
            k, _, v = line.partition("=")
            env[k] = v
    if not any("Hostx86" in p or "HostX86" in p for p in env.get("PATH", "").split(";")):
        sys.exit("vcvars32 capture failed: cl not on the captured PATH")
    # CreateProcess resolves the executable against the PARENT's PATH, not
    # the child env being passed -- so find cl.exe now and invoke it by
    # absolute path, or every spawn dies FileNotFoundError despite a correct
    # child PATH.
    import shutil
    cl = shutil.which("cl", path=env["PATH"])
    if not cl:
        sys.exit("cl.exe not found on the captured PATH")
    env["SM64DS_CL"] = cl
    return env


def check_batch(files, env):
    """{file: first_error or None} for one cl /Zs invocation over many files."""
    cmd = [env["SM64DS_CL"], "/nologo", "/Zs", "/W0",
           f"/I{INCLUDE}", f"/I{PORT}", "/DSM64DS_PLATFORM_PC"]
    # //cpp-marked .c files and .cpp files are C++; plain .c stays C. cl
    # decides by extension, which matches the repo convention.
    cmd += [str(f) for f in files]
    p = subprocess.run(cmd, capture_output=True, text=True, errors="replace",
                       env=env, cwd=str(SRC))
    results = {f: None for f in files}
    current = None
    err_re = re.compile(r"^(.*?)\((\d+)\): (?:fatal )?error (C\d+):")
    for line in (p.stdout + p.stderr).splitlines():
        m = err_re.match(line.strip())
        if not m:
            continue
        path = m.group(1)
        code = m.group(3)
        for f in files:
            if str(f).endswith(path.split("\\")[-1]) or path.endswith(f.name):
                if results[f] is None:
                    results[f] = code
                break
    return results


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--jobs", type=int, default=8)
    ap.add_argument("--batch", type=int, default=40)
    ap.add_argument("--detail", default=None)
    ap.add_argument("--limit", type=int, default=40)
    args = ap.parse_args()

    files = sorted(p for p in SRC.rglob("*") if p.suffix in (".c", ".cpp"))
    print(f"host frontier: {len(files)} source files, cl /Zs, 32-bit")
    env = vc_env()

    batches = [files[i:i + args.batch] for i in range(0, len(files), args.batch)]
    results = {}
    done = 0
    with ThreadPoolExecutor(max_workers=args.jobs) as ex:
        for r in ex.map(lambda b: check_batch(b, env), batches):
            results.update(r)
            done += 1
            if done % 25 == 0:
                print(f"  ... {done}/{len(batches)} batches", file=sys.stderr)

    ok = [f for f, e in results.items() if e is None]
    bad = {f: e for f, e in results.items() if e is not None}
    buckets = Counter()
    members = defaultdict(list)
    for f, code in bad.items():
        b = BUCKETS.get(code, code)
        buckets[b] += 1
        members[b].append(f)

    pct = 100.0 * len(ok) / len(files) if files else 0
    print(f"\ncompiles for host: {len(ok)} / {len(files)}  ({pct:.1f}%)\n")
    for b, n in buckets.most_common():
        print(f"  {n:6}  {b}")

    if args.detail:
        for b in [k for k in members if args.detail.lower() in k.lower()]:
            print(f"\n--- {b} ({len(members[b])}) ---")
            for f in sorted(members[b])[:args.limit]:
                print(f"  {f.relative_to(REPO)}")
            if len(members[b]) > args.limit:
                print(f"  ... {len(members[b]) - args.limit} more")


if __name__ == "__main__":
    main()
