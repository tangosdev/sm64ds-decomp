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


ERR_RE = re.compile(r"^(.*?)\((\d+)\): (?:fatal )?error (C\d+):")


def _same_file(path, f):
    """True if an error line's own path names batch member f."""
    base = path.replace("/", "\\").rsplit("\\", 1)[-1]
    return base == f.name


def parse_cl_transcript(text, files):
    """{file: (code, raised_in) or None} from one cl /Zs transcript.

    Factored out of check_batch so the attribution can be unit-tested
    without invoking cl. `files` must be in the same order they were
    passed on cl's command line.

    cl compiles a multi-source invocation sequentially in argument order,
    and echoes each source's bare file name on its own line right before
    it starts compiling that source -- that echo is the only reliable
    per-file cursor. An error line's OWN path names wherever the error
    was actually raised, which is the source itself for an ordinary
    error but a HEADER's path for an error raised while expanding a
    header that source pulled in; attributing by that path alone (the
    old bug) makes such a source read as clean, because the error's path
    matches no batch member. So every error line here is attributed to
    `current` -- the most recently echoed batch member -- and the
    error's own path is kept only as `raised_in`, set when it differs
    from `current` (i.e. the error was raised inside a header), and left
    None when the error was raised in the source itself. The
    first-error-per-source rule still holds: a second error against a
    source that already has one is dropped, whether or not it was raised
    in a header. Because attribution no longer depends on the error's
    own path resolving to a batch member, a source whose every error is
    raised in a header still lands in `bad`, not in `ok`.
    """
    order = list(files)
    results = {f: None for f in order}
    next_idx = 0
    current = None
    for raw in text.splitlines():
        line = raw.strip()
        if next_idx < len(order) and line == order[next_idx].name:
            current = order[next_idx]
            next_idx += 1
            continue
        m = ERR_RE.match(line)
        if not m:
            continue
        if current is None:
            # An error line before this batch's first echo names no
            # source cl has told us it is compiling; nothing to charge
            # it to.
            continue
        if results[current] is not None:
            continue
        path, code = m.group(1), m.group(3)
        raised_in = None if _same_file(path, current) else path
        results[current] = (code, raised_in)
    return results


def check_batch(files, env):
    """{file: (code, raised_in) or None} for one cl /Zs invocation."""
    cmd = [env["SM64DS_CL"], "/nologo", "/Zs", "/W0",
           f"/I{INCLUDE}", f"/I{PORT}", "/DSM64DS_PLATFORM_PC"]
    # //cpp-marked .c files and .cpp files are C++; plain .c stays C. cl
    # decides by extension, which matches the repo convention.
    cmd += [str(f) for f in files]
    p = subprocess.run(cmd, capture_output=True, text=True, errors="replace",
                       env=env, cwd=str(SRC))
    return parse_cl_transcript(p.stdout + p.stderr, files)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--jobs", type=int, default=8)
    ap.add_argument("--batch", type=int, default=40)
    ap.add_argument("--detail", default=None)
    ap.add_argument("--limit", type=int, default=40)
    args = ap.parse_args()

    files = sorted(p for p in SRC.rglob("*") if p.suffix in (".c", ".cpp"))
    print(f"host frontier: {len(files)} source files, cl /Zs, 32-bit")

    # asm-hatch TUs (`asm void f(...) { ... }`, mwccarm syntax) can never
    # compile on a host and SHOULD not: they are DS hardware operations --
    # CP15 cache management (host no-op), block copy/fill (memcpy), context
    # switch (threads seam). Classify them as HAL-owned up front instead of
    # letting them read as 98 syntax defects; they are the HAL backlog.
    hal_owned = []
    rest = []
    asm_re = re.compile(r"^\s*asm\s+\w", re.M)
    for f in files:
        try:
            head = f.read_text(errors="replace")
        except OSError:
            rest.append(f)
            continue
        (hal_owned if asm_re.search(head) else rest).append(f)
    files = rest
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
    for f, (code, raised_in) in bad.items():
        b = BUCKETS.get(code, code)
        buckets[b] += 1
        members[b].append((f, raised_in))

    total = len(files) + len(hal_owned)
    pct = 100.0 * len(ok) / len(files) if files else 0
    print(f"\ncompiles for host: {len(ok)} / {len(files)}  ({pct:.1f}%)"
          f"   [+ {len(hal_owned)} asm-hatch TUs, HAL-owned on host, "
          f"excluded from the denominator]\n")
    for b, n in buckets.most_common():
        print(f"  {n:6}  {b}")

    if args.detail:
        for b in [k for k in members if args.detail.lower() in k.lower()]:
            print(f"\n--- {b} ({len(members[b])}) ---")
            for f, raised_in in sorted(members[b], key=lambda t: t[0])[:args.limit]:
                line = f"  {f.relative_to(REPO)}"
                if raised_in:
                    line += f"  (raised in {raised_in})"
                print(line)
            if len(members[b]) > args.limit:
                print(f"  ... {len(members[b]) - args.limit} more")


if __name__ == "__main__":
    main()
