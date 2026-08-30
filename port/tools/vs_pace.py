"""HOW FAST DOES AN ONLINE VS MATCH ACTUALLY RUN? Run rel0215, lane vslag.

The owner played the two-window VS demo through the live relay and reported it
"fully working, good music, extremely laggy, very slow and low framerate". That
is a pacing report, and nothing in the tree measured pacing on the VS path:
vs_online_proof.py proves a session FORMS and that input crosses the wire, and
net_proof.py's rungN6/N7 measure wall time for a fixed frame count on the LEVEL
path with the run UNPACED. Neither answers "what frame rate does a player get".

So this file measures the frame clock itself, over the arms that separate the
candidate causes:

    solo1      one instance, VS map, two players, NO comms at all
    solo2      TWO instances of that at once, still no comms -- the cost of
               running two copies of the game on one desk, on its own
    local0     a pair through a relay on 127.0.0.1, input delay 0
    local      the same pair at this build's default for the mode
    live0      a pair through the live relay, input delay 0 -- THE DEMO
    live       the same pair at the default

solo1 vs solo2 is the two-instances-on-one-PC share. local vs live is the
round trip's share. live0 vs live is what the input pipeline buys back.

EVERY ARM IS PACED (SM64DS_PACE_SELFTEST=1) and that is not a detail. A
selftest is normally unpaced, and the input pipeline's budget is N frames of
WALL TIME -- frame R waits for round R-N, so the wire gets N frame periods to
deliver it. Paced, N=4 is 133 ms of cover; unpaced over 6 ms frames it is 24 ms
and the pipeline starves. An unpaced arm cannot measure pacing, and would
report the mitigation doing a fraction of what it does in a player's hands.

The numbers come from the game's own frame clock: SM64DS_TRACE_PACE=1 prints
one [fps] line per 120 frames carrying that window's rate and the distribution
behind it. A mean cannot tell a steady 30 fps from one that stalls every fourth
frame, which is the entire question here.

READING THE TABLE. The budget is 33.30 ms (the VS scene writes divider 2). A
frame period at the budget is the pacer holding 30 fps and is the best possible
result; a period well over it is the game failing to keep up and the player
sees 1000/period. p95 is per 120-frame window, and the column reports the
median window with the worst window beside it, because aggregating a percentile
across windows exactly would need every sample and the windows are equal size.

    python port/tools/vs_pace.py                    all six arms
    python port/tools/vs_pace.py --arms live0,live  just the before and after
    python port/tools/vs_pace.py --frames 360       shorter
"""

import argparse
import os
import re
import socket
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(ROOT, "build", "vs_pace")

LIVE_RELAY = "tangos.dev:41234"

# THE LOCAL UDP BASE, derived from this process rather than fixed. Two reasons,
# both measured in this tree: a fixed base makes any other instance of the game
# on the machine fail the parent's bind with winsock 10048, and this desk has
# Windows UDP exclusion ranges at 50000-50059 and 58434-58733 that answer a
# bind with winsock 10013. 41800 and up is clear of both and of the relay's own
# 41234.
def pid_port_base():
    return 41800 + (os.getpid() % 100) * 8


def free_port():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(("127.0.0.1", 0))
    p = s.getsockname()[1]
    s.close()
    return p


def start_local_relay(port):
    relay = os.path.join(ROOT, "port", "tools", "relay", "relay.py")
    env = dict(os.environ)
    env["SM64DS_RELAY_PORT"] = str(port)
    env["SM64DS_RELAY_BIND"] = "127.0.0.1"
    log = open(os.path.join(OUT, "relay.log"), "wb")
    p = subprocess.Popen([sys.executable, relay], env=env,
                         stdout=log, stderr=subprocess.STDOUT)
    p._logfile = log
    time.sleep(1.0)
    return p


def vs_env(run_dir, tag, frames, vs_map):
    """The arms' common half: a paced, traced, quiet, muted VS selftest."""
    e = M.env_base(ROOT, run_dir, tag)
    # env_base names a LEVEL because every rung in mp2_proof measures one. A VS
    # boot names its own destination and must not carry a second.
    e.pop("SM64DS_LEVEL", None)
    e["SM64DS_VS_MAP"] = str(vs_map)
    e["SM64DS_VS_PLAYERS"] = "2"
    e["SM64DS_WINDOW_SELFTEST"] = str(frames)
    e["SM64DS_PACE_SELFTEST"] = "1"
    e["SM64DS_TRACE_PACE"] = "1"
    return e


def comms_env(e, role, relay, code, port_base, input_delay, counters=False):
    e["SM64DS_COMMS_ROLE"] = role
    e["SM64DS_COMMS_RELAY"] = relay
    e["SM64DS_COMMS_CODE"] = code
    e["SM64DS_COMMS_PORT"] = str(port_base)
    # THE COUNTER LINE IS OPT-IN, AND OFF BY DEFAULT HERE ON PURPOSE.
    #
    # `starved` -- the verdict on whether the input delay is deep enough for
    # the path -- only reaches the log through comms_loopback_report, which
    # walk_window calls from inside the SM64DS_COMMS_FANOUT block. That block
    # runs the ROM's own steps 0x16 and 0x17 every frame and prints four lines
    # per frame with SM64DS_COMMS_REPORT on. Both are real per-frame work, and
    # a pacing arm that carried them would be quoting a frame rate for a
    # configuration no player runs.
    #
    # So: --counters is a SEPARATE run, for reading the counters. The frame
    # rates in the table come from the clean arms.
    if counters:
        e["SM64DS_COMMS_FANOUT"] = "1"
        e["SM64DS_COMMS_REPORT"] = "1"
    if input_delay is not None:
        e["SM64DS_COMMS_INPUT_DELAY"] = str(input_delay)
    return e


FPS_RE = re.compile(
    r"^\[fps\] (\d+) frames in ([\d.]+)s = ([\d.]+) fps \(divider (\d+), "
    r"budget ([\d.]+)ms\) frame ms avg ([\d.]+) p50 ([\d.]+) p95 ([\d.]+) "
    r"max ([\d.]+)$", re.M)


def windows(text, skip=1):
    """The [fps] windows in one log, minus the first.

    SKIPPING THE FIRST IS NOT TIDYING. Window 1 contains the frames either side
    of the session coming up -- the relay handshake, the JOIN/ACCEPT, the
    pipeline filling -- and a one-off startup cost measured as steady-state
    pacing would be a different claim than the one this file makes. Every arm
    drops the same window, so the arms stay comparable, and --frames is chosen
    so several windows remain.
    """
    rows = []
    for m in FPS_RE.finditer(text):
        rows.append(dict(n=int(m.group(1)), wall=float(m.group(2)),
                         fps=float(m.group(3)), budget=float(m.group(5)),
                         avg=float(m.group(6)), p50=float(m.group(7)),
                         p95=float(m.group(8)), max=float(m.group(9))))
    return rows[skip:]


def summarise(rows):
    if not rows:
        return None
    frames = sum(r["n"] for r in rows)
    wall = sum(r["wall"] for r in rows)
    p95s = sorted(r["p95"] for r in rows)
    return dict(windows=len(rows), frames=frames, wall=wall,
                fps=(frames / wall) if wall else 0.0,
                avg=wall * 1000.0 / frames if frames else 0.0,
                p50=sorted(r["p50"] for r in rows)[len(rows) // 2],
                p95=p95s[len(rows) // 2], p95_worst=p95s[-1],
                max=max(r["max"] for r in rows),
                budget=rows[0]["budget"])


def report_line(text):
    """The carrier's LAST counter line, which only --counters produces.

    Tagged [loopback:<scene>] by comms_loopback_report, not [comms:loopback]:
    the [comms:loopback] prefix belongs to the carrier's own narration (open,
    accepted, closed) and carries no counters.
    """
    m = None
    for m in re.finditer(r"^\[loopback:\w+\] .*indelay=.*$", text, re.M):
        pass
    return m.group(0) if m else ""


def num(line, key, cast=int, default=-1):
    m = re.search(re.escape(key) + r"=(-?[\w.]+)", line)
    try:
        return cast(m.group(1)) if m else default
    except ValueError:
        return default


# ---------------------------------------------------------------------------
# THE ARMS
# ---------------------------------------------------------------------------
def arm_solo(name, a, copies):
    """One or two instances, no comms at all."""
    procs, dirs = [], []
    for i in range(copies):
        d = os.path.join(OUT, "%s_%d" % (name, i + 1))
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
        dirs.append(d)
    for i, d in enumerate(dirs):
        e = vs_env(d, "%s%d" % (name, i + 1), a.frames, a.map)
        procs.append(M.spawn(EXE, d, e, os.path.join(d, "run.log")))
    rcs = [M.finish(p, a.timeout) for p in procs]
    texts = [M.text(os.path.join(d, "run.log")) for d in dirs]
    # The FIRST instance is the one measured either way, so solo1 and solo2
    # compare the same process under two different machine loads.
    return dict(rcs=rcs, rows=windows(texts[0]), texts=texts, dirs=dirs)


def arm_pair(name, a, relay, input_delay, code):
    dp = os.path.join(OUT, name + "_p1")
    dc = os.path.join(OUT, name + "_p2")
    for d in (dp, dc):
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    base = pid_port_base()
    ep = comms_env(vs_env(dp, name + "p1", a.frames, a.map),
                   "parent", relay, code, base, input_delay, a.counters)
    ec = comms_env(vs_env(dc, name + "p2", a.frames, a.map),
                   "child", relay, code, base, input_delay, a.counters)
    lp, lc = os.path.join(dp, "run.log"), os.path.join(dc, "run.log")
    pp = M.spawn(EXE, dp, ep, lp)
    time.sleep(0.6)
    pc = M.spawn(EXE, dc, ec, lc)
    rcs = [M.finish(pp, a.timeout), M.finish(pc, a.timeout)]
    tp, tc = M.text(lp), M.text(lc)
    # THE CHILD IS THE HONEST END TO READ, the same reason net_proof.pace gives:
    # a parent completes a round the instant its own block is staged if it
    # already holds the child's, so a parent alone can look fast while the
    # session as a whole crawls.
    return dict(rcs=rcs, rows=windows(tc), texts=[tp, tc], dirs=[dp, dc],
                report_p=report_line(tp), report_c=report_line(tc))


ARMS = ["solo1", "solo2", "local0", "local", "live0", "live"]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--frames", type=int, default=600,
                    help="selftest frames per instance; five 120-frame "
                         "windows at the default, of which the first is "
                         "dropped as session bring-up")
    ap.add_argument("--map", type=int, default=0)
    ap.add_argument("--arms", default=",".join(ARMS))
    ap.add_argument("--relay", default=LIVE_RELAY)
    ap.add_argument("--code", default="VSPACE")
    ap.add_argument("--timeout", type=int, default=900,
                    help="per-instance wall limit")
    ap.add_argument("--counters", action="store_true",
                    help="turn the ROM fan-out and the transport's counter "
                         "line on, so `starved` reaches the log. It is real "
                         "per-frame work: read the counters from this run, "
                         "never the frame rate")
    a = ap.parse_args()

    os.makedirs(OUT, exist_ok=True)
    if not os.path.exists(EXE):
        print("no exe at " + EXE)
        return 2
    print("exe %s  sha %s" % (EXE, M.sha(EXE)))
    print("live relay %s, local UDP base %d" % (a.relay, pid_port_base()))

    want = [w.strip() for w in a.arms.split(",") if w.strip()]
    results = {}
    proc = None
    local_relay = None
    try:
        for name in want:
            print("\n== %s ==" % name, flush=True)
            t0 = time.time()
            if name == "solo1":
                r = arm_solo(name, a, 1)
            elif name == "solo2":
                r = arm_solo(name, a, 2)
            elif name in ("local0", "local"):
                if local_relay is None:
                    port = free_port()
                    proc = start_local_relay(port)
                    local_relay = "127.0.0.1:%d" % port
                    print("   local relay on " + local_relay)
                r = arm_pair(name, a, local_relay,
                             0 if name.endswith("0") else None,
                             (a.code + "L" + name[-1])[:8])
            elif name in ("live0", "live"):
                r = arm_pair(name, a, a.relay,
                             0 if name.endswith("0") else None,
                             (a.code + "W" + name[-1])[:8])
            else:
                print("   unknown arm, skipped")
                continue
            r["s"] = summarise(r["rows"])
            r["elapsed"] = time.time() - t0
            results[name] = r
            print("   rc=%s  windows=%d  wall=%.1fs"
                  % (r["rcs"], len(r["rows"]), r["elapsed"]), flush=True)
            for key in ("report_p", "report_c"):
                if r.get(key):
                    print("   " + r[key].strip())
            if r["s"]:
                s = r["s"]
                print("   %.2f fps   avg %.2f  p50 %.2f  p95 %.2f  max %.2f ms"
                      % (s["fps"], s["avg"], s["p50"], s["p95"], s["max"]),
                      flush=True)
    finally:
        if proc:
            proc.terminate()
            proc._logfile.close()

    # ---- the table -------------------------------------------------------
    hdr = ("%-8s %7s %7s %8s %8s %8s %8s %8s %7s"
           % ("arm", "fps", "avg", "p50", "p95", "p95wst", "max", "budget",
              "starved"))
    lines = [hdr, "-" * len(hdr)]
    for name in want:
        r = results.get(name)
        if not r or not r["s"]:
            lines.append("%-8s  no [fps] windows" % name)
            continue
        s = r["s"]
        st = num(r.get("report_c", ""), "starved", int, -1)
        lines.append("%-8s %7.2f %7.2f %8.2f %8.2f %8.2f %8.2f %8.2f %7s"
                     % (name, s["fps"], s["avg"], s["p50"], s["p95"],
                        s["p95_worst"], s["max"], s["budget"],
                        st if st >= 0 else "-"))
    table = "\n".join(lines)
    print("\n" + table)

    path = os.path.join(OUT, "pace.txt")
    with open(path, "w") as f:
        f.write("# %s\n# exe sha %s\n# frames %d per instance, map %d, "
                "relay %s\n# ms columns are FRAME PERIODS from the game's own "
                "[fps] windows,\n# first window dropped as bring-up. p95 is "
                "per 120-frame window:\n# the column is the median window, "
                "p95wst the worst one.\n"
                % (time.strftime("%Y-%m-%d %H:%M:%S"), M.sha(EXE), a.frames,
                   a.map, a.relay))
        f.write(table + "\n")
        for name in want:
            r = results.get(name)
            if r and r.get("report_c"):
                f.write("\n%s child: %s\n" % (name, r["report_c"].strip()))
    print("\nwrote " + path)
    return 0


if __name__ == "__main__":
    sys.exit(main())
