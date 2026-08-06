#!/usr/bin/env python3
"""Does the PC port still COMPILE AND LINK against the current src/ and include/?

This exists because of a real two-day outage. #1049 ("Give the shared
declaration headers C linkage") was correct for the ROM link and broke 13 of
the port's 14 gate binaries: four HAL definitions were deliberately
C++-linkage, the decl headers started emitting C-linkage references, and the
two no longer met. Nothing noticed, because nothing in the decomp toolchain
builds port/.

What the existing gates do NOT cover:
  tools/port_refcheck.py   catches a src/ RENAME stranding a port/ reference.
                           #1049 renamed nothing -- every reference was valid,
                           the LINKAGE changed.
  port/tools/port_evidence.py
                           asks whether the code is proven to be the game's.
                           Orthogonal: unproven code links fine, and proven
                           code can fail to link.
Only linking catches a linkage break. So: link.

Cost is small because it is incremental -- ninja rebuilds only what the change
touched. The default target set is the two cheapest binaries, which is enough:
#1049 broke both.

Usage:
  python port/tools/port_linkcheck.py            # smoke + smoke_heap
  python port/tools/port_linkcheck.py --all      # every gate binary
  python port/tools/port_linkcheck.py --targets smoke_actor
"""
import argparse
import os
import re
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from host_frontier import find_vcvars   # noqa: E402  (same vswhere lookup)

REPO = Path(__file__).resolve().parents[2]
PORT = REPO / "port"
BUILD = REPO / "build" / "port"

# The cheapest pair that still spans the HAL seams #1049 broke: gate 1 pulls
# os_time/shims, gate 2 pulls the heap bridges. Both failed in that outage.
DEFAULT_TARGETS = ["smoke", "smoke_heap"]


def msvc_env():
    """Environment with the 32-bit MSVC toolchain, or None if unavailable.

    Returns None rather than exiting: a contributor without VS should be told
    the check was SKIPPED, never allowed to read silence as a pass.
    """
    try:
        vcvars = find_vcvars()
    except SystemExit:
        return None
    parent = dict(os.environ)
    installer = str(Path(vcvars).parents[3] / "Installer")
    if Path(installer, "vswhere.exe").exists():
        parent["PATH"] = installer + os.pathsep + parent.get("PATH", "")
    out = subprocess.run(["cmd", "/c", "call", vcvars, ">nul", "&&", "set"],
                         capture_output=True, text=True, errors="replace",
                         env=parent)
    env = dict(parent)
    for line in out.stdout.splitlines():
        if "=" in line:
            k, _, v = line.partition("=")
            env[k] = v
    # LIB is what makes this a LINK check rather than a compile check -- without
    # it link.exe dies on kernel32.lib and every target "fails" for a reason
    # that has nothing to do with the code.
    if not env.get("LIB") or not any(
            "Hostx86" in p or "HostX86" in p for p in env.get("PATH", "").split(";")):
        return None
    return env


# MSVC spells the two linkages differently, and the difference is a trap:
#   C:    unresolved external symbol _func_02059650 referenced in function ...
#   C++:  unresolved external symbol "char data_020a4d38" (?data_020a4d38@@3DA)
#         referenced in function ...
# A \S+ capture stops at the first space, so every C++ symbol collapses to the
# token "char / "int / "public: and distinct symbols dedupe into one. That is
# the same undercount this report exists to prevent -- and #1049, the outage
# this tool was written for, was a C-vs-C++ linkage flip, so both shapes are
# guaranteed to appear here. Anchor on ' referenced in' instead.
SYM_RE = re.compile(r"unresolved external symbol (.+?)(?= referenced in function|\s*$)")

# Only the trailing bucket is capped, and never silently -- see summarize().
OTHER_CAP = 40


def summarize(out):
    """Group ninja/MSVC failure output into an untruncated diagnosis.

    The previous version printed the first 40 matching lines and stopped. On a
    real linkage break that is about a third of the output, with no marker that
    anything was dropped -- a reviewer reads 40 lines, counts 6 broken binaries
    and moves on, when 13 are broken. (That happened, during review of the very
    PR that added this file.)

    The output is redundant rather than large: each unresolved symbol repeats
    once per binary needing it, and each binary compiles its own objects under
    CMakeFiles/<target>.dir/, so raw line-dedupe does NOT collapse them -- the
    path prefixes differ. Grouping does. The two buckets that carry the
    diagnosis are small once deduplicated (~11 symbols, ~14 binaries) and are
    never capped; only the open-ended tail is, and it says so.
    """
    keep = [ln.rstrip() for ln in out.splitlines()
            # Surface the diagnosis, not the 4KB link command line carrying it.
            if ("error" in ln.lower() or "FAILED" in ln)
            and "cmd.exe /C" not in ln and "link.exe /nologo" not in ln]
    if not keep:
        return out[-3000:]

    # Buckets are assigned by CONSUMPTION, not by parallel membership tests: a
    # line the symbol regex fails to parse must fall through to `other` rather
    # than vanish from both. Anything that stops matching still gets printed.
    syms, bins, other = {}, {}, {}
    for ln in keep:
        m = SYM_RE.search(ln)
        if m:
            syms.setdefault(m.group(1).strip(), None)
        elif "LNK1120" in ln:
            bins.setdefault(ln.strip(), None)
        else:
            # Compile-stage breaks (C1083, C2065), LNK1104, ninja's FAILED
            # edges. Deduped too: one bad shared header emits the identical
            # error once per target that includes it.
            other.setdefault(ln.strip(), None)

    parts = []
    if syms:
        parts.append(f"unresolved symbols ({len(syms)}):")
        parts += [f"  {s}" for s in syms]
    if bins:
        parts.append(f"\nbinaries that failed to link ({len(bins)}):")
        parts += [f"  {b}" for b in bins]
    if other:
        parts.append(f"\nother diagnostics ({len(other)}):")
        parts += [f"  {o}" for o in list(other)[:OTHER_CAP]]
        if len(other) > OTHER_CAP:
            parts.append(f"  ... {len(other) - OTHER_CAP} more suppressed -- "
                         f"for the full set: ninja -C {BUILD} -k 0")
    # Totals last and always honest, so a capped tail still cannot be read as
    # the whole story.
    parts.append(f"\nsummary: {len(bins)} binaries failed to link, "
                 f"{len(syms)} distinct unresolved symbols, "
                 f"{len(other)} other diagnostic line(s)")
    return "\n".join(parts)


def tool(env, name):
    for d in env.get("PATH", "").split(os.pathsep):
        p = Path(d) / f"{name}.exe"
        if p.exists():
            return str(p)
    # pip install cmake ninja -- Python's Scripts dir is usually not on PATH.
    import sysconfig
    p = Path(sysconfig.get_path("scripts")) / f"{name}.exe"
    return str(p) if p.exists() else None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--all", action="store_true", help="build every gate binary")
    ap.add_argument("--targets", nargs="*", help="explicit ninja targets")
    args = ap.parse_args()

    env = msvc_env()
    if env is None:
        print("port-linkcheck: SKIPPED -- no 32-bit MSVC toolchain found.")
        print("                This did NOT pass; it did not run.")
        return 0

    cmake, ninja = tool(env, "cmake"), tool(env, "ninja")
    if not cmake or not ninja:
        print("port-linkcheck: SKIPPED -- cmake/ninja not found "
              "(VS component 'C++ CMake tools for Windows', or "
              "pip install cmake ninja).")
        print("                This did NOT pass; it did not run.")
        return 0

    if not (BUILD / "build.ninja").exists():
        print(f"port-linkcheck: configuring {BUILD.relative_to(REPO)}")
        r = subprocess.run([cmake, "-S", str(PORT), "-B", str(BUILD), "-G", "Ninja",
                            "-DCMAKE_BUILD_TYPE=Release",
                            f"-DCMAKE_MAKE_PROGRAM={ninja}"],
                           env=env, capture_output=True, text=True, errors="replace")
        if r.returncode:
            print(r.stdout[-3000:] + r.stderr[-3000:])
            print("port-linkcheck: FAILED to configure")
            return 1

    targets = args.targets or ([] if args.all else DEFAULT_TARGETS)
    # -k 0 = keep going after failures. Ninja's default (-k 1) stops at the
    # first one, so a run reports whichever failures happened to be in flight
    # and hides the rest. That turns diagnosis into fix-one-discover-more:
    # three linkage breaks looked like the whole story, and fixing them
    # revealed nine more behind them. A link check should enumerate the damage
    # in one pass.
    r = subprocess.run([ninja, "-C", str(BUILD), "-k", "0"] + targets,
                       env=env, capture_output=True, text=True, errors="replace")
    if r.returncode:
        print(summarize(r.stdout + r.stderr))
        print(f"\nport-linkcheck: FAILED -- the port does not build against "
              f"the current src/ and include/.")
        return 1

    built = ", ".join(targets) if targets else "all gate binaries"
    print(f"port-linkcheck: OK ({built})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
