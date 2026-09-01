"""VSMERCY -- measure the port's VS star-collect mercy window against the ROM's.

THE ROM'S RULE, derived from the matched source and quoted here because every
number this script prints is only meaningful next to it:

  func_ov002_020d82f0 is the predicate every damage entry asks first --
  Player::Hurt, Player::Shock, Player::Burn, and func_ov002_020d8360, the
  player-vs-player hit handler Player::Behavior reaches through
  func_ov002_020d869c. It refuses on mInvincibleTimer +0x6a0, mIsMega +0x703,
  mCapFlags +0x73c, mIsVanish +0x6fb, mIsTakingDamage +0x708 and mIsNoControl
  +0x709.

  A VS star collect arms the last of those. func_ov002_020e8ef0 (the collect)
  calls Player::SetNoControlState with the kind func_ov002_020e73ac returns in
  VS, which is 2. St_NoControl_Init runs Player_DisableInteraction
  (mIsNoControl = 1, body-collider flags +0x2ec |= 4) and the kind-2 step
  func_ov002_020c7ff8 also clears mIsBodyClsnEnabled +0x713 and plays anim
  0x19. func_ov002_020c92fc leaves the state on Player::FinishedAnim, and
  Player::ChangeState puts all three back the same call.

  So the window is EXACTLY the star-get animation, with no timer and no
  post-animation grace. mInvincibleTimer's 0x24 (36 frames) belongs to the
  HURT states, not to a collect.

WHAT THIS MEASURES

  pass 1  the window: one VS instance, a scripted collect, SM64DS_VS_MERCY=every,
          and the frame count from mIsNoControl 0 -> 1 -> 0.
  pass 2  the refusal: the same run with SM64DS_MERCY_HIT on frames inside and
          just past that window, asking the REAL Player::Hurt to land a hit.
  pass 3  the wire: two instances of the same build in one session, the same
          scripted collect on both, and whether the collecting player's body
          carries the same window on the console that is NOT collecting.

Quiet and muted through mp2_proof.env_base, which also scrubs an inherited
SM64DS_* so a lane's own knobs cannot leak in.

    python port/tools/vsmercy_probe.py [--frames N] [--map 0] [--collect 200]
"""
import argparse
import os
import re
import subprocess
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M  # noqa: E402

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(ROOT, "build", "vsmercy")

MERCY = re.compile(
    r"^\[mercy\] f(\d+) p(\d+) canhurt=(\d+) noctl=(\d+) kind=(\d+) "
    r"bodyclsn=(\d+) cflag4=(\d+) inv=(\d+) takingdmg=(\d+) anim=(\d+) "
    r"step=(\d+) state=([0-9a-f]+) animlen=(\d+) animcur=(\d+)", re.M)
HIT = re.compile(r"^\[mercyhit\] f(\d+) gate=(-?\d+) Hurt=(-?\d+) "
                 r"noctl=(\d+) anim=(\d+)", re.M)


# THE PORT BASE IS DERIVED FROM THIS PROCESS'S PID, never a constant. The
# owner's live desk pair is 51765 and the 58434..58733 band is spoken for.
def port_base():
    return 43000 + (os.getpid() % 700) * 8


def mercy_rows(text, slot=0):
    out = []
    for m in MERCY.finditer(text):
        if int(m.group(2)) != slot:
            continue
        out.append(dict(f=int(m.group(1)), canhurt=int(m.group(3)),
                        noctl=int(m.group(4)), kind=int(m.group(5)),
                        bodyclsn=int(m.group(6)), cflag4=int(m.group(7)),
                        inv=int(m.group(8)), dmg=int(m.group(9)),
                        anim=int(m.group(10)), step=int(m.group(11)),
                        state=m.group(12), animlen=int(m.group(13)),
                        animcur=int(m.group(14))))
    return out


def window(rows):
    """-> (first frame with noctl set, first frame after it clears) or None."""
    start = end = None
    for r in rows:
        if start is None and r["noctl"]:
            start = r["f"]
        elif start is not None and end is None and not r["noctl"]:
            end = r["f"]
            break
    if start is None:
        return None
    return (start, end)


def base_env(run_dir, vsmap, frames, players, collect, breakall):
    e = M.env_base(ROOT, run_dir, "vsmercy")
    e.pop("SM64DS_LEVEL", None)          # a VS boot names its own destination
    e["SM64DS_VS_MAP"] = str(vsmap)
    e["SM64DS_WINDOW_SELFTEST"] = str(frames)
    e["SM64DS_VS_PLAYERS"] = str(players)
    e["SM64DS_VS_BREAKALL"] = str(breakall)
    e["SM64DS_STAR_TRIGGER"] = str(collect)
    e["SM64DS_VS_MERCY"] = "every"
    return e


def run_solo(tag, frames, vsmap, collect, breakall, hits=None):
    d = os.path.join(OUT, tag)
    os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
    log = os.path.join(d, "run.log")
    e = base_env(d, vsmap, frames, 2, collect, breakall)
    if hits:
        e["SM64DS_MERCY_HIT"] = ",".join(str(h) for h in hits)
    rc = M.finish(M.spawn(EXE, d, e, log), 900)
    return rc, M.text(log), log


def run_pair(tag, frames, vsmap, collect, breakall, base, stagger=1.0):
    procs, logs = [], []
    for k in range(2):
        d = os.path.join(OUT, tag, "p%d" % k)
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
        lg = os.path.join(d, "run.log")
        e = base_env(d, vsmap, frames, 2, collect, breakall)
        e["SM64DS_COMMS_ROLE"] = "parent" if k == 0 else "child"
        e["SM64DS_COMMS_PORT"] = str(base)
        e["SM64DS_COMMS_REPORT"] = "1"
        e["SM64DS_SYNC"] = "1"
        if k:
            e["SM64DS_COMMS_SLOT"] = "1"
        logs.append(lg)
        procs.append(M.spawn(EXE, d, e, lg))
        if k == 0:
            time.sleep(stagger)
    res = []
    for k in range(2):
        rc = M.finish(procs[k], 900)
        res.append((rc, M.text(logs[k]), logs[k]))
    return res


def report(tag, text, log):
    rows = mercy_rows(text)
    print("  %s: %d [mercy] rows for slot 0, log %s" % (tag, len(rows), log))
    w = window(rows)
    if not w:
        print("  %s: NO mercy window -- mIsNoControl never set on slot 0" % tag)
        return None
    start, end = w
    if end is None:
        print("  %s: window OPENS at f%d and never closes inside the run"
              % (tag, start))
        return (start, None, None)
    print("  %s: window f%d..f%d = %d frames armed" % (tag, start, end - 1,
                                                       end - start))
    inside = [r for r in rows if start <= r["f"] < end]
    bad = [r for r in inside if r["canhurt"]]
    print("  %s: canhurt was 0 on %d/%d frames inside the window%s"
          % (tag, len(inside) - len(bad), len(inside),
             "" if not bad else "  <-- LEAKS at f" +
             ",".join(str(r["f"]) for r in bad[:6])))
    if inside:
        r0 = inside[0]
        print("  %s: at the open -- kind=%d anim=%u step=%u bodyclsn=%u "
              "cflag4=%u" % (tag, r0["kind"], r0["anim"], r0["step"],
                             r0["bodyclsn"], r0["cflag4"]))
    after = [r for r in rows if r["f"] >= end][:1]
    if after:
        print("  %s: the frame it closes -- f%d canhurt=%d inv=%u bodyclsn=%u"
              % (tag, after[0]["f"], after[0]["canhurt"], after[0]["inv"],
                 after[0]["bodyclsn"]))
    return (start, end, end - start)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--frames", default="900")
    ap.add_argument("--map", type=int, default=0)
    ap.add_argument("--collect", type=int, default=300)
    ap.add_argument("--breakall", type=int, default=200)
    ap.add_argument("--port", type=int, default=0)
    ap.add_argument("--skip-pair", action="store_true")
    args = ap.parse_args()

    print("vsmercy_probe: exe %s" % EXE, flush=True)

    print("\n== pass 1: the window, one instance", flush=True)
    rc, t, lg = run_solo("solo", args.frames, args.map, args.collect,
                         args.breakall)
    print("  rc=%d" % rc)
    w = report("solo", t, lg)

    if w and w[1]:
        start, end, n = w
        # inside, at the last armed frame, and two frames past the close
        hits = sorted({start + 2, max(start + 2, end - 2), end + 1, end + 3})
        print("\n== pass 2: the refusal, hits at %s"
              % ",".join(str(h) for h in hits), flush=True)
        rc2, t2, lg2 = run_solo("hits", args.frames, args.map, args.collect,
                                args.breakall, hits=hits)
        print("  rc=%d  log %s" % (rc2, lg2))
        w2 = report("hits", t2, lg2)
        for m in HIT.finditer(t2):
            f, gate, r, noctl, anim = (int(m.group(1)), int(m.group(2)),
                                       int(m.group(3)), int(m.group(4)),
                                       int(m.group(5)))
            where = "?"
            if w2 and w2[1]:
                where = ("INSIDE" if w2[0] <= f < w2[1]
                         else "AFTER" if f >= w2[1] else "BEFORE")
            print("  hit f%d (%s): gate=%d Hurt=%d noctl=%d anim=%d"
                  % (f, where, gate, r, noctl, anim))

    if args.skip_pair:
        return

    base = args.port or port_base()
    print("\n== pass 3: the wire, two instances, port base %d" % base,
          flush=True)
    res = run_pair("pair", args.frames, args.map, args.collect, args.breakall,
                   base)
    ws = []
    for k, (rc, t, lg) in enumerate(res):
        print("  window %d rc=%d" % (k, rc))
        ws.append(report("pair-p%d" % k, t, lg))
    if all(ws) and all(x and x[1] for x in ws):
        d_open = ws[1][0] - ws[0][0]
        d_len = ws[1][2] - ws[0][2]
        print("  the two consoles: open differs by %d frame(s), length differs "
              "by %d frame(s)" % (d_open, d_len))
    else:
        print("  the two consoles: at least one console has NO window -- see "
              "the per-window lines above")


if __name__ == "__main__":
    main()
