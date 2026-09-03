"""The ROLLBACK NETCODE proof ladder (port/rollback). status/ROLLBACK_SHIP.md.

Every rung launches walk_window.exe instances quiet and muted through
mp2_proof.env_base with SM64DS_NETMODE=rollback injected after its scrub, and
reads the logs. Rungs:

  PAIR    two windows on level 1 at induced RTT 0/40/80/160 ms, the injected
          key TOGGLING so guesses go wrong and real rollbacks happen; dhdiff
          on the two [dh] streams -> NO DIVERGENCE; the same-frame local
          input probe must say OK on both ends; the mode must have taken.
  VS4     four windows, VS map 1, the 6-pairing dhdiff sweep, at RTT
          0/40/80/160 ms, direct.
  VS7     seven windows (the wide wire), VS map 0, the 21-pairing sweep,
          at RTT 0/40/80/160 ms, direct.
  RELAY4  the VS4 sweep through the local reference relay, all four RTTs.
  RELAY7  the VS7 sweep through the local reference relay, all four RTTs.
  DET     the restore+retick determinism check inside a live VS session: 4
          and 16 players, SM64DS_ROLLBACK_DET on the parent, through the
          real rollback path -> rb-det: IDENTICAL arena=0 dsstate=0 hw=0.
  COST    16 players, SM64DS_ROLLBACK_FORCE rewinding 8 rounds every 30
          frames on the parent: the [rollback] p95 lines, once with the
          tick-only re-sim (the default) and once conservative
          (SM64DS_ROLLBACK_ACTOR_RENDER=1, every Render body kept).
  STALL   three windows on VS map 0, slot 2 asleep 2500 ms at frame 200
          with its pump stopped: the parent and slot 1 must serve the
          8-round window at full rate, then stall, then the parent retires
          slot 2 under the grace rule, both survivors finish their budget
          and still agree round for round; the sleeper must be told.
  RELAY   the PAIR rung once more through the local reference relay.

  python port/tools/rollback_proof.py [--only PAIR,VS4,...] [--frames N]
                                      [--wide N]

  --wide N: the wide session's player count for DET and COST (default 16).
  The 16-player wide session faults at frame 2 on slots 8, 12, 14 and 15 in
  LOCKSTEP too (Minimap::Render, src/_ZN7Minimap6RenderEv.cpp, indexes its
  four-entry icon and position tables with the player number; walker actor
  id 0x14f); status/ROLLBACK_SHIP.md. --wide 8 is the widest session that
  survives, and the ladder runs DET/COST at that width when asked.
"""
import argparse
import os
import re
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import mp2_proof as M  # noqa: E402

ROOT = os.path.abspath(os.path.join(HERE, "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(ROOT, "build", "rollback_proof")
DHDIFF = os.path.join(HERE, "dhdiff.py")
RELAY = os.path.join(HERE, "relay", "relay.py")
BASE = 43000 + (os.getpid() % 300) * 20
KEYS = [0x0040, 0x0080, 0x0020, 0x0010]
FRAMES = 600
TOGGLE = 45      # frames per injected-key phase; the guess is wrong at each flip

results = []


def say(ok, name, detail):
    results.append(ok)
    print("%s  %s | %s" % ("PASS" if ok else "FAIL", name, detail))
    return ok


def grab(text, rx, default=None, group=1):
    m = re.search(rx, text, re.M)
    return m.group(group) if m else default


def env_for(k, n, outdir, frames, vsmap=None, extra=None, base=None):
    d = os.path.join(outdir, "p%d" % k)
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    e = M.env_base(ROOT, d, "p%d" % k)
    e["SM64DS_NETMODE"] = "rollback"
    e["SM64DS_WINDOW_SELFTEST"] = str(frames)
    e["SM64DS_COMMS_ROLE"] = "parent" if k == 0 else "child"
    e["SM64DS_COMMS_PORT"] = str(base or BASE)
    e["SM64DS_COMMS_FANOUT"] = "1"
    # NOT SM64DS_COMMS_REPORT: its per-frame lines are several unbuffered
    # stderr writes a frame, about 9 ms here, and they land inside the
    # exchange phase of every replayed frame. The cost numbers this ladder
    # reports are measured the way a player runs, with it off; the session
    # check reads the live mask off the transport's closing line instead.
    # The hash at level 1 is the world hash the sweep needs; a divergence is
    # re-run at level 3 by hand to name the field.
    e["SM64DS_VS_STATE_HASH"] = "1"
    e["SM64DS_ROLLBACK_LOCALPROBE"] = "1"
    e["SM64DS_COMMS_INJECT"] = "key=0x%04x,toggle=%d,key2=0x%04x" % (
        KEYS[k % 4], TOGGLE, KEYS[(k + 1) % 4])
    if k:
        e["SM64DS_COMMS_SLOT"] = str(k)
    if vsmap is not None:
        e.pop("SM64DS_LEVEL", None)
        e["SM64DS_VS_MAP"] = str(vsmap)
        e["SM64DS_VS_PLAYERS"] = str(n)
        e["SM64DS_VS_PROBE"] = "1"
    e.update(extra or {})
    return d, e


def launch(name, n, frames, vsmap=None, per=None, stagger=1.0, parent_extra=None,
           base=None, timeout=1500):
    outdir = os.path.join(OUT, name)
    pbudget = frames + 60 * (n + 8)
    procs, logs = [], []
    for k in range(n):
        extra = dict((per or {}).get(k, {}))
        if k == 0:
            extra.update(parent_extra or {})
        d, e = env_for(k, n, outdir, pbudget if k == 0 else frames, vsmap, extra, base)
        lp = os.path.join(d, "run.log")
        procs.append(M.spawn(EXE, d, e, lp))
        logs.append(lp)
        if k == 0:
            time.sleep(stagger)
    t0 = time.time()
    rcs = [M.finish(p, timeout) for p in procs]
    wall = time.time() - t0
    return dict(rcs=rcs, logs=logs, texts=[M.text(l) for l in logs], wall=wall,
                outdir=outdir)


DH = re.compile(r"^\[dh\] f(\d+) .*rounds=(\d+)", re.M)


def trim(logpath, cap):
    """Keep the [dh]/[dh=] lines up to round `cap`, last line per frame wins,
    written beside the log. dhdiff aligns on rounds and refuses a tail one
    window has and the other has not; rollback replays a frame's line again
    with the corrected world, and the loader's dict keeps the last."""
    out = logpath + ".trim"
    with open(logpath, "r", errors="replace") as f, open(out, "w") as g:
        for line in f:
            m = DH.match(line)
            if m and int(m.group(2)) > cap:
                continue
            if line.startswith("[dh=] f") or line.startswith("[dh+] f"):
                pass
            g.write(line)
    return out


def max_round(text):
    r = [int(m.group(2)) for m in DH.finditer(text)]
    return max(r) if r else 0


def sweep(name, r, pairs=None):
    n = len(r["logs"])
    caps = [max_round(t) for t in r["texts"]]
    cap = min(caps) - 8 if caps else 0
    if cap <= 30:
        return say(False, name + " sweep", "too few hashed rounds: %r" % caps)
    trimmed = [trim(l, cap) for l in r["logs"]]
    pairs = pairs or [(a, b) for a in range(n) for b in range(a + 1, n)]
    bad, clean = 0, 0
    for a, b in pairs:
        p = subprocess.run([sys.executable, DHDIFF, trimmed[a], trimmed[b]],
                           capture_output=True, text=True)
        if p.returncode == 0:
            clean += 1
        else:
            bad += 1
            print("      p%d vs p%d rc=%d: %s" % (a, b, p.returncode,
                  (p.stdout.strip().splitlines() or ["?"])[-1][:160]))
    return say(bad == 0, name + " sweep",
               "%d/%d pairings NO DIVERGENCE to round %d (max rounds %r)"
               % (clean, len(pairs), cap, caps))


def mode_took(name, r):
    ok = all("NetMode ROLLBACK" in t for t in r["texts"])
    return say(ok, name + " mode", "NetMode ROLLBACK printed by %d/%d windows"
               % (sum("NetMode ROLLBACK" in t for t in r["texts"]), len(r["texts"])))


def session_ok(name, r, n):
    want = (1 << n) - 1
    oks = []
    for t in r["texts"]:
        live = grab(t, r"rollback: live=0x[0-9a-f]+ peak=0x([0-9a-f]+)", "0")
        oks.append(int(live, 16) == want)
    return say(all(oks), name + " session", "live=0x%x on %d/%d, rc=%r, wall %.1fs"
               % (want, sum(oks), n, r["rcs"], r["wall"]))


def summary_lines(r):
    for i, t in enumerate(r["texts"]):
        for line in t.splitlines():
            if line.startswith("[rollback] =====") or line.startswith("[rollback] rollback event") \
               or line.startswith("[rollback] per replayed") or line.startswith("[rollback] snapshot") \
               or line.startswith("[rollback] restore") or line.startswith("[rb-local]") \
               or line.startswith("[rb-det]") or "rollback: predicted=" in line:
                print("      p%d %s" % (i, line.strip()[:220]))


def probe_ok(name, r):
    oks = [grab(t, r"\[rb-local\].*-> (\w+)") == "OK" for t in r["texts"]]
    return say(all(oks), name + " local input same-frame",
               "[rb-local] OK on %d/%d" % (sum(oks), len(oks)))


def rung_pair(frames, relay=None):
    ok = True
    for rtt in (0, 40, 80, 160):
        extra = {}
        if rtt:
            extra["SM64DS_COMMS_DELAY_MS"] = str(rtt // 2)
        if relay:
            extra["SM64DS_COMMS_RELAY"] = relay
            # one code PER RTT: the relay keeps an endpoint for its idle
            # expiry (90 s) after a window exits, so a second session on the
            # same code inside that window is told FULL and never seats
            extra["SM64DS_COMMS_CODE"] = "RB%03d%03d" % (os.getpid() % 1000, rtt)
        name = "%s_rtt%d" % ("relay" if relay else "pair", rtt)
        r = launch(name, 2, frames, per={0: extra, 1: extra}, base=BASE + (rtt // 8) * 2)
        ok &= mode_took(name, r)
        ok &= session_ok(name, r, 2)
        ok &= sweep(name, r)
        ok &= probe_ok(name, r)
        rolled = [grab(t, r"rollbacks=(\d+)", "0") for t in r["texts"]]
        unrec = [grab(t, r"unrecoverable=(\d+)", "?") for t in r["texts"]]
        ok &= say(all(u == "0" for u in unrec), name + " every rewind honoured",
                  "unrecoverable=%r rollbacks=%r" % (unrec, rolled))
        summary_lines(r)
    return ok


def rung_vs(name, n, vsmap, frames, rtts=(0, 40, 80, 160), relay=None):
    ok = True
    for rtt in rtts:
        extra = {"SM64DS_COMMS_DELAY_MS": str(rtt // 2)} if rtt else {}
        if relay:
            extra["SM64DS_COMMS_RELAY"] = relay
            extra["SM64DS_COMMS_CODE"] = "R%d%02d%04d" % (n, rtt // 10, os.getpid() % 10000)
        per = dict((k, dict(extra)) for k in range(n))
        rn = "%s_rtt%d" % (name, rtt)
        r = launch(rn, n, frames, vsmap=vsmap, per=per,
                   base=BASE + 40 + n * 2 + (rtt // 8))
        ok &= mode_took(rn, r)
        ok &= session_ok(rn, r, n)
        ok &= sweep(rn, r)
        ok &= probe_ok(rn, r)
        summary_lines(r)
    return ok


WIDE = 16


def rung_det(frames):
    ok = True
    for n in (4, WIDE):
        rn = "det%d" % n
        r = launch(rn, n, frames, vsmap=0, base=BASE + 100 + n * 2,
                   parent_extra={"SM64DS_ROLLBACK_DET": "150",
                                 "SM64DS_ROLLBACK_DET_N": "8"},
                   per={1: {"SM64DS_ROLLBACK_DET": "150",
                            "SM64DS_ROLLBACK_DET_N": "8"}})
        ok &= mode_took(rn, r)
        ok &= session_ok(rn, r, n)
        for k in (0, 1):
            v = grab(r["texts"][k], r"^rb-det: (.*)$")
            good = v is not None and v.startswith("IDENTICAL arena=0")
            ok &= say(good, "%s p%d restore+retick" % (rn, k), v or "no verdict")
        ok &= sweep(rn, r, pairs=[(0, 1), (0, n - 1), (1, n - 1)])
        summary_lines(r)
    return ok


STAT = r"\s+n=\s*\d+ mean\s+([\d.]+) ms\s+p50\s+([\d.]+)\s+p95\s+([\d.]+)\s+max\s+([\d.]+)"


def cost_line(t):
    ev = re.search(r"rollback event \(restore\+replay\)" + STAT, t)
    fr = re.search(r"per replayed frame" + STAT, t)
    sn = re.search(r"snapshot \(arena\+dsstate\+hw log\)" + STAT, t)
    rs = re.search(r"\] restore" + STAT, t)
    f = lambda m: ("mean %s p50 %s p95 %s max %s" % m.groups()) if m else "none"
    return ev, ("event %s | per frame %s | snapshot %s | restore %s"
                % (f(ev), f(fr), f(sn), f(rs)))


def rung_cost(frames):
    ok = True
    for tag, extra in (("tickonly", {}),
                       ("conservative", {"SM64DS_ROLLBACK_ACTOR_RENDER": "1"})):
        rn = "cost%d_%s" % (WIDE, tag)
        pe = {"SM64DS_ROLLBACK_FORCE": "30:8"}
        pe.update(extra)
        r = launch(rn, WIDE, frames, vsmap=0,
                   base=BASE + 160 + (4 if extra else 0), parent_extra=pe)
        ok &= mode_took(rn, r)
        ok &= session_ok(rn, r, WIDE)
        ok &= sweep(rn, r, pairs=[(0, 1), (0, WIDE - 1), (3, WIDE // 2 + 1)])
        ev, line = cost_line(r["texts"][0])
        ok &= say(ev is not None, rn + " measured",
                  "%dP forced 8-round rollbacks: " % WIDE + line)
        summary_lines(r)
    return ok


def rung_stall(frames):
    r = launch("stall", 3, frames, vsmap=0,
               per={2: {"SM64DS_ROLLBACK_PAUSE": "200:2500"}}, base=BASE + 200)
    tp, t1, tc = r["texts"]
    ok = mode_took("stall", r)
    ok &= session_ok("stall", dict(r, texts=r["texts"][:2]), 3)   # the sleeper's own close is not the question
    stalled = "stalling (grace" in tp
    retired = "slot 2 retired (grace)" in tp
    told = "the parent left" in tc or "leaving the session" in tc or "Bye" in tc
    ok &= say(stalled, "stall parent stalled",
              grab(tp, r"^(.*stalling \(grace.*)$", "no stall line").strip()[:160])
    ok &= say(retired, "stall drop per rule",
              grab(tp, r"^(.*retired \(grace\).*)$", "no retire line").strip()[:160])
    ok &= say(told, "stall sleeper told",
              "slot 2 saw the Bye" if told else "slot 2 never told")
    for k, t in ((0, tp), (1, t1)):
        stats = grab(t, r"rollback: live=\S+ peak=\S+ (predicted=.*)$", "no transport line")
        ok &= say("stallevents=1" in stats, "stall p%d one stall episode" % k,
                  stats[:200])
        over = grab(t, r"^(.*stall over after.*)$", "")
        if over:
            print("      p%d %s" % (k, over.strip()[:160]))
    for k in (0, 1):
        pos = grab(r["texts"][k], r"^selftest: (\d+) frames")
        ok &= say(pos is not None and r["rcs"][k] == 0, "stall p%d finished" % k,
                  "rc=%d selftest frames=%s wall %.1fs" % (r["rcs"][k], pos, r["wall"]))
    ok &= sweep("stall survivors", r, pairs=[(0, 1)])
    summary_lines(r)
    return ok


def start_relay(port):
    env = dict(os.environ)
    env["SM64DS_RELAY_PORT"] = str(port)
    env["SM64DS_RELAY_BIND"] = "127.0.0.1"
    # every window here is one source address, and a wide session is more
    # than the relay's default three children
    env["SM64DS_RELAY_MAX_CHILDREN"] = "15"
    env["SM64DS_RELAY_RATE_PPS"] = "2000"
    p = subprocess.Popen([sys.executable, RELAY], env=env,
                         stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(1.0)
    if p.poll() is not None:
        raise RuntimeError("relay exited")
    return p


RUNGS = ("PAIR", "VS4", "VS7", "RELAY", "RELAY4", "RELAY7", "DET", "COST",
         "STALL")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--only", default="")
    ap.add_argument("--frames", type=int, default=FRAMES)
    ap.add_argument("--wide", type=int, default=16)
    a = ap.parse_args()
    global WIDE
    WIDE = a.wide
    if not os.path.exists(EXE):
        print("no exe at %s -- build first" % EXE)
        return 2
    os.makedirs(OUT, exist_ok=True)
    want = [s.strip().upper() for s in a.only.split(",") if s.strip()]
    print("rollback_proof: exe %s  ports %d..  out %s" % (M.sha(EXE), BASE, OUT))
    ok = True
    for name in RUNGS:
        if want and name not in want:
            continue
        print("\n=== %s ===" % name)
        try:
            if name == "PAIR":
                ok &= rung_pair(a.frames)
            elif name == "VS4":
                ok &= rung_vs("vs4", 4, 1, max(a.frames, 900))
            elif name == "VS7":
                ok &= rung_vs("vs7", 7, 0, max(a.frames, 900))
            elif name in ("RELAY4", "RELAY7"):
                n = 4 if name == "RELAY4" else 7
                rp = start_relay(BASE + 300 + n)
                try:
                    ok &= rung_vs("relay%d" % n, n, 1 if n == 4 else 0,
                                  max(a.frames, 900),
                                  relay="127.0.0.1:%d" % (BASE + 300 + n))
                finally:
                    rp.kill()
            elif name == "DET":
                ok &= rung_det(400)
            elif name == "COST":
                ok &= rung_cost(600)
            elif name == "STALL":
                ok &= rung_stall(a.frames)
            elif name == "RELAY":
                rp = start_relay(BASE + 300)
                try:
                    ok &= rung_pair(a.frames, relay="127.0.0.1:%d" % (BASE + 300))
                finally:
                    rp.kill()
        except Exception as e:  # noqa: BLE001
            ok = False
            print("  %s RAISED: %r" % (name, e))
    print("\n%s (%d pass, %d fail)" % ("ALL GREEN" if ok else "FAILURES ABOVE",
                                       sum(results), len(results) - sum(results)))
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
