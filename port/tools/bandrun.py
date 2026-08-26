"""The band feature's scene runner. Every run goes through port/tools/gapproof.py scene_run, so
NO_FOCUS + CREATE_NO_WINDOW + SW_SHOWMINNOACTIVE are all set by the tool -- the
desk stays silent. Windowed runs (crossing proofs) pass SM64DS_SCENE_WINDOW=1
through extra_env, which scene_run re-adds AFTER its own pop, and the frame
budget ends the run by itself.

    python tools/bandrun.py head  <scene> <frames> <out.bmp> [ON|OFF] [k=v ...]
    python tools/bandrun.py win   <scene> <frames> <out.bmp> [ON|OFF] [k=v ...]

The stdout+stderr of the run is written next to the bmp as <out>.log, raw.
"""
import os
import sys

TREE = os.path.abspath(os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "..", ".."))
sys.path.insert(0, os.path.join(TREE, "port", "tools"))
import gapproof as G  # noqa: E402


def main(argv):
    mode = argv[0]
    scene = int(argv[1])
    frames = int(argv[2])
    out = os.path.abspath(argv[3])
    arm = argv[4].upper() if len(argv) > 4 else "ON"
    env = {}
    if arm == "OFF":
        env["SM64DS_BAND_FIX"] = "0"
    elif arm != "ON":
        raise SystemExit("arm must be ON or OFF, got %r" % arm)
    for kv in argv[5:]:
        k, _, v = kv.partition("=")
        env[k] = v
    if mode == "win":
        env["SM64DS_SCENE_WINDOW"] = "1"
    elif mode != "head":
        raise SystemExit("mode must be head or win")
    os.makedirs(os.path.dirname(out) or ".", exist_ok=True)
    r = G.scene_run(scene, frames, out, extra_env=env)
    log = os.path.splitext(out)[0] + ".log"
    with open(log, "w", encoding="utf-8", errors="replace") as f:
        f.write("# ARGV %r\n# EXTRA_ENV %r\n# rc=%d\n" % (argv, env,
                                                          r.returncode))
        f.write(r.stdout or "")
        f.write(r.stderr or "")
    print("rc=%d  %s  (%s)" % (r.returncode, out,
                               "exists" if os.path.exists(out) else "NO BMP"))
    return r.returncode


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
