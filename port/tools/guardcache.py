#!/usr/bin/env python3
"""Run the pre-configure guards once per CHANGE instead of once per BUILD.

WHY THIS EXISTS.  build-port.cmd runs eleven python guards before it reaches the
first cmake line.  Measured on this tree they cost about eleven minutes of wall
clock, every build, whether or not anything they inspect moved -- and there are
ten port lanes building on one box.  That is the single largest fixed cost in
the port build and none of it is compilation.

WHAT THIS DOES NOT DO.  It does not skip a check, soften a check, or turn a red
guard green.  Every guard still runs, in the same order, with the same
arguments, and its stdout, stderr and exit code reach build-port.cmd byte for
byte.  Two things change:

  1. THE GUARDS RUN IN PARALLEL.  They are independent -- each reads the tree
     and reports -- so eleven serial runs become one wave.  The wall cost of the
     block becomes the slowest guard, not the sum.

  2. A GREEN VERDICT IS REMEMBERED, KEYED BY ITS INPUTS.  If nothing a guard
     reads has changed since it last passed, its recorded output is replayed.
     The key covers the guard script, every other file in port/tools (any of
     them can be imported), the python and MSVC toolchain identity, and a
     stat signature -- path, size, mtime -- of every tree the guard reads.  A
     RED VERDICT IS NEVER STORED: a failing guard re-runs on every build until
     it passes, so nobody can cache their way past a refusal.

THE SHAPE IN build-port.cmd.  One `--prime` call ahead of the block runs the
whole wave; each guard's own line becomes `--replay`, which prints what the wave
recorded and exits with the recorded code.  If the wave did not run, or did not
cover that guard, `--replay` RUNS THE GUARD FOR REAL.  Losing the cache costs
time and never correctness.

    guardcache.py --prime                  run the wave (parallel, cached)
    guardcache.py --replay NAME.py [args]  replay one verdict, or run it
    guardcache.py --stats                  what is cached right now
    guardcache.py --clear                  drop the stored verdicts
"""

import argparse
import base64
import concurrent.futures
import hashlib
import json
import os
import re
import subprocess
import sys
import time

TOOLS = os.path.dirname(os.path.abspath(__file__))
PORT = os.path.dirname(TOOLS)
REPO = os.path.dirname(PORT)
CACHE = os.path.join(REPO, "build", "port", "guardcache")
RUNFILE = os.path.join(CACHE, "run.json")

SRC_EXT = (".c", ".cpp", ".h", ".hpp", ".txt", ".cmd", ".py")

# What each guard READS, as tree roots relative to the repo.  A guard whose
# entry is [] is hermetic: it builds its fixtures in a temporary directory and
# reads nothing from the tree, so its verdict can only change when its own code
# or the toolchain changes -- both of which are in the key already.  Everything
# else names the roots it walks.  OVER-naming a root costs cache hits and never
# correctness; UNDER-naming one would be a silent stale verdict, so when in
# doubt the root goes in.
INPUT_ROOTS = {
    # port/ whole: the slice lists, CMakeLists.txt, hal/, unmatched/, tests/
    # and ntr/ all sit directly under it, and naming the parent means a lane
    # that adds a new subdirectory of sources is covered without an edit here.
    "closestplayer_guard.py": ["src", "port"],
    "inferred_stub_guard.py": ["src", "port"],
    "vtablerows.py --reconstruct": ["src", "port", "config",
                                    "extracted/overlays"],
    "stategen.py --reconstruct": ["src", "port", "config",
                                  "extracted/overlays"],
}

# The wave.  Same list, same order, same arguments as build-port.cmd's block;
# --prime cross-checks it against that file and refuses to run if they drift,
# so this table cannot quietly stop covering a guard the build still runs.
WAVE = [
    ["closestplayer_guard.py"],
    ["inferred_stub_guard.py"],
    ["closure.py", "--selftest"],
    ["facegen.py", "--selftest"],
    ["mapdiff.py", "--selftest"],
    ["vtablerows.py", "--selftest"],
    ["vtablerows.py", "--reconstruct"],
    ["stategen.py", "--selftest"],
    ["stategen.py", "--reconstruct"],
    ["alternatename_guard.py", "--selftest"],
    ["gxband_guard.py", "--selftest"],
    ["tailjump_guard.py", "--selftest"],
]


def argv_key(argv):
    return " ".join(argv)


# ---------------------------------------------------------------- signatures

_sig_memo = {}


def _walk_sig(path):
    """path + size + mtime of every interesting file under a root.  Content is
    never read: a stat per file is two orders of magnitude cheaper and any edit
    moves the mtime.  A touched-but-identical file costs one re-run."""
    if path in _sig_memo:
        return _sig_memo[path]
    h = hashlib.sha256()
    if os.path.isfile(path):
        st = os.stat(path)
        h.update(("%s|%d|%d\n" % (os.path.basename(path), st.st_size,
                                  st.st_mtime_ns)).encode())
    elif os.path.isdir(path):
        rows = []
        for root, dirs, files in os.walk(path):
            dirs[:] = sorted(d for d in dirs
                             if d not in (".git", "__pycache__", "build"))
            for f in sorted(files):
                if not f.endswith(SRC_EXT):
                    continue
                full = os.path.join(root, f)
                try:
                    st = os.stat(full)
                except OSError:
                    continue
                rows.append("%s|%d|%d" % (os.path.relpath(full, path),
                                          st.st_size, st.st_mtime_ns))
        h.update("\n".join(rows).encode())
    else:
        h.update(b"absent")
    out = h.hexdigest()
    _sig_memo[path] = out
    return out


def tools_sig():
    """Every .py in port/tools, by content.  Any of them can be imported by any
    guard, so a change to one invalidates the lot.  Cheap: ~40 small files."""
    if "@tools" in _sig_memo:
        return _sig_memo["@tools"]
    h = hashlib.sha256()
    for name in sorted(os.listdir(TOOLS)):
        if not name.endswith(".py"):
            continue
        with open(os.path.join(TOOLS, name), "rb") as fh:
            h.update(name.encode())
            h.update(hashlib.sha256(fh.read()).digest())
    _sig_memo["@tools"] = h.hexdigest()
    return _sig_memo["@tools"]


def env_sig():
    """The toolchain the guards shell out to.  facegen probes undname, the map
    guards build fixture binaries; a compiler upgrade can legitimately change a
    verdict, so it is part of the key."""
    parts = [sys.version, os.environ.get("VCToolsVersion", ""),
             os.environ.get("VSCMD_VER", ""), os.environ.get("VSCMD_ARG_TGT_ARCH", "")]
    return hashlib.sha256("|".join(parts).encode()).hexdigest()


def key_for(argv):
    h = hashlib.sha256()
    h.update(b"guardcache-v2\n")
    h.update(argv_key(argv).encode())
    h.update(tools_sig().encode())
    h.update(env_sig().encode())
    roots = INPUT_ROOTS.get(argv_key(argv))
    if roots is None:
        roots = INPUT_ROOTS.get(argv[0], [])
    for r in roots:
        h.update(("%s=%s\n" % (r, _walk_sig(os.path.join(REPO, r)))).encode())
    return h.hexdigest()


# ---------------------------------------------------------------- running

def run_guard(argv):
    """Run one guard and keep its output as RAW BYTES.

    Bytes, not text, and replayed to sys.stdout.buffer below, because a guard's
    output already carries Windows line endings: decoding it to str and writing
    it back through sys.stdout would translate every \n a second time and
    replay \r\r\n where the guard wrote \r\n. The replay has to be the same
    bytes the guard produced, so it is stored base64 and written unchanged.
    """
    script = os.path.join(TOOLS, argv[0])
    t0 = time.time()
    proc = subprocess.run([sys.executable, script] + list(argv[1:]),
                          cwd=REPO, stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE)
    return {"rc": proc.returncode,
            "out": base64.b64encode(proc.stdout).decode ("ascii"),
            "err": base64.b64encode(proc.stderr).decode ("ascii"),
            "secs": round(time.time() - t0, 2)}


def cache_path(key):
    return os.path.join(CACHE, key + ".json")


def load_cached(key):
    try:
        with open(cache_path(key), encoding="utf-8") as fh:
            blob = json.load(fh)
        if blob.get("rc") == 0:
            return blob
    except (OSError, ValueError):
        pass
    return None


def store_cached(key, res):
    # A RED VERDICT IS NEVER STORED.  A guard that refuses must refuse again on
    # the next build, and on every build until the tree is fixed.
    if res["rc"] != 0:
        return
    os.makedirs(CACHE, exist_ok=True)
    tmp = cache_path(key) + ".tmp"
    with open(tmp, "w", encoding="utf-8", newline="\n") as fh:
        json.dump(res, fh)
    os.replace(tmp, cache_path(key))


def emit(res):
    sys.stdout.buffer.write(base64.b64decode(res["out"]))
    sys.stdout.buffer.flush()
    sys.stderr.buffer.write(base64.b64decode(res["err"]))
    sys.stderr.buffer.flush()
    return res["rc"]


# ---------------------------------------------------------------- drift check

CMD_RE = re.compile(r'^\s*python\s+"%~dp0tools\\([A-Za-z0-9_]+\.py)"(.*)$')


def wave_from_build_script():
    """The guard list build-port.cmd actually runs, up to the cmake line.

    Read rather than assumed, so this file cannot silently stop covering a
    guard someone adds to the build script.  Returns None if the script cannot
    be parsed, which makes --prime skip the drift check rather than block a
    build over its own bookkeeping."""
    path = os.path.join(PORT, "build-port.cmd")
    try:
        with open(path, encoding="utf-8", errors="replace") as fh:
            lines = fh.read().splitlines()
    except OSError:
        return None
    wave = []
    for raw in lines:
        if raw.strip().startswith("cmake -S"):
            break
        m = CMD_RE.match(raw)
        if not m:
            continue
        name, rest = m.group(1), m.group(2)
        if name in ("guardcache.py", "tierscan.py"):
            continue
        args = [a for a in rest.split() if not a.startswith(">")]
        wave.append([name] + args)
    return wave


def replay_wave_from_script():
    """Same, but reads the --replay form this change installs."""
    path = os.path.join(PORT, "build-port.cmd")
    try:
        with open(path, encoding="utf-8", errors="replace") as fh:
            lines = fh.read().splitlines()
    except OSError:
        return None
    wave = []
    for raw in lines:
        if raw.strip().startswith("cmake -S"):
            break
        m = re.match(r'^\s*python\s+"%~dp0tools\\guardcache\.py"\s+--replay\s+(.*)$',
                     raw)
        if not m:
            continue
        args = [a for a in m.group(1).split() if not a.startswith(">")]
        if args:
            wave.append(args)
    return wave


# ---------------------------------------------------------------- commands

def cmd_prime(jobs):
    declared = replay_wave_from_script()
    if declared:
        want = [argv_key(a) for a in declared]
        have = [argv_key(a) for a in WAVE]
        missing = [w for w in want if w not in have]
        if missing:
            sys.stderr.write("guardcache: build-port.cmd replays guards this "
                             "table does not cover: %s\n" % ", ".join(missing))
            return 1
        wave = declared
    else:
        wave = WAVE

    t0 = time.time()
    keys = {argv_key(a): key_for(a) for a in wave}
    sig_secs = time.time() - t0

    pending, results = [], {}
    for a in wave:
        k = argv_key(a)
        hit = load_cached(keys[k])
        if hit is not None:
            hit = dict(hit)
            hit["cached"] = True
            results[k] = hit
        else:
            pending.append(a)

    if pending:
        with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
            futs = {pool.submit(run_guard, a): argv_key(a) for a in pending}
            for fut in concurrent.futures.as_completed(futs):
                k = futs[fut]
                res = fut.result()
                res["cached"] = False
                results[k] = res
                store_cached(keys[k], res)

    os.makedirs(CACHE, exist_ok=True)
    tmp = RUNFILE + ".tmp"
    with open(tmp, "w", encoding="utf-8", newline="\n") as fh:
        json.dump(results, fh)
    os.replace(tmp, RUNFILE)

    total = time.time() - t0
    hits = sum(1 for r in results.values() if r["cached"])
    sys.stderr.write("guardcache: %d guards, %d cached, %d run, "
                     "%.1fs wall (%.1fs signatures)\n"
                     % (len(results), hits, len(results) - hits, total, sig_secs))
    for a in wave:
        r = results[argv_key(a)]
        sys.stderr.write("guardcache:   %-42s %s rc=%d %6.2fs\n"
                         % (argv_key(a), "HIT " if r["cached"] else "run ",
                            r["rc"], r.get("secs", 0.0)))
    # --prime never decides the build.  Each guard's own line reports its own
    # verdict, in its own place, so a refusal reads exactly as it always did.
    return 0


def cmd_replay(argv):
    try:
        with open(RUNFILE, encoding="utf-8") as fh:
            results = json.load(fh)
    except (OSError, ValueError):
        results = {}
    res = results.get(argv_key(argv))
    if res is None:
        # No wave, or a guard the wave did not cover: run it for real.
        res = run_guard(argv)
        store_cached(key_for(argv), res)
    return emit(res)


def cmd_stats():
    n = 0
    if os.path.isdir(CACHE):
        n = len([f for f in os.listdir(CACHE) if f.endswith(".json")
                 and f != "run.json"])
    print("guardcache: %d stored verdicts in %s" % (n, CACHE))
    try:
        with open(RUNFILE, encoding="utf-8") as fh:
            for k, v in sorted(json.load(fh).items()):
                print("  %-42s rc=%d %s %.2fs"
                      % (k, v["rc"], "HIT" if v.get("cached") else "run",
                         v.get("secs", 0.0)))
    except (OSError, ValueError):
        pass
    return 0


def cmd_clear():
    if os.path.isdir(CACHE):
        for f in os.listdir(CACHE):
            if f.endswith(".json"):
                os.remove(os.path.join(CACHE, f))
    print("guardcache: cleared")
    return 0


def main(argv=None):
    ap = argparse.ArgumentParser(add_help=False)
    ap.add_argument("--prime", action="store_true")
    ap.add_argument("--replay", nargs=argparse.REMAINDER)
    ap.add_argument("--stats", action="store_true")
    ap.add_argument("--clear", action="store_true")
    ap.add_argument("--jobs", type=int, default=min(12, (os.cpu_count() or 4)))
    ap.add_argument("-h", "--help", action="store_true")
    args = ap.parse_args(argv)
    if args.help:
        print(__doc__)
        return 0
    if args.clear:
        return cmd_clear()
    if args.stats:
        return cmd_stats()
    if args.prime:
        return cmd_prime(args.jobs)
    if args.replay:
        return cmd_replay(args.replay)
    print(__doc__)
    return 2


if __name__ == "__main__":
    sys.exit(main())
