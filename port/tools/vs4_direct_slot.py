"""DIRECT-MODE regression test for the ACCEPT assignment-bit clear (run vs4p).

THE LINE THIS EXISTS FOR. hal/comms_loopback.cpp's roster announce sends the
grown live mask to every child as an ACCEPT, and it MUST clear the slot
assignment bit:

    a.have = ((unsigned)(g_input_delay & 0xFF) << 8);   // 0x80000000 clear

because the child's adoption arm

    if (g_net_mode == kNetDirect && (p.have & 0x80000000u)) {
        const int assigned = (int)(p.have & 0xFF);
        ... g_slot = assigned;

would otherwise move an already-seated child onto the slot the broadcast names.
Set the bit and four direct-mode windows collapse onto one slot -- the reviewer
proved that by breaking it, three consoles ended up on slot 3.

WHY vs4_proof.py CANNOT COVER THIS, and why this is a separate file. That proof
runs LOOPBACK, where the adoption arm is unreachable (`g_net_mode ==
kNetDirect` is false) no matter what the broadcast carries, and it pins every
child's slot besides -- so it stays ALL GREEN with the bit set wrong. The most
dangerous line in the lane had zero regression coverage. This runs the four
windows in DIRECT mode so the arm is live.

THE TEST REFUSES TO PASS VACUOUSLY. A direct-mode run that silently fell back
to loopback would make the hazard unreachable and the test meaningless, so
rung 1 asserts every window actually opened DIRECT, and rung 2 asserts the
parent genuinely learned all three children over that path. Only then does
rung 3 -- no child adopted a broadcast slot -- mean anything.

    python port/tools/vs4_direct_slot.py [--frames N] [--map 0..3]
"""
import argparse
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import mp2_proof as M  # noqa: E402

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)),
                                    "..", ".."))
EXE = os.path.join(ROOT, "build", "port", "walk_window.exe")
OUT = os.path.join(ROOT, "build", "vs4_direct_slot")

KEYS = (0x0040, 0x0080, 0x0020, 0x0010)

SEAT = re.compile(r"\[a2\] VS: (\d+) players, I am slot (\d+)")
DIRECT = re.compile(r"open\(mode=\d+\) as \w+, slot \d+, udp .* DIRECT")
LEARNED = re.compile(r"direct: learned slot (\d+)")
# The adoption line a bit-set broadcast would produce on a child.
ADOPT = re.compile(r"the parent assigned slot (\d+) \(proposed (\d+)\)")


def port_base():
    # PID-derived, clear of the owner's desk pair (51765) and the 58434..58733
    # band, stepped by 8 so slot k = base + k stays clear.
    return 44000 + (os.getpid() % 700) * 8


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--frames", default="600")
    ap.add_argument("--map", type=int, default=0)
    ap.add_argument("--stagger", type=float, default=1.0)
    ap.add_argument("--port", type=int, default=0)
    args = ap.parse_args()

    base = args.port or port_base()
    print("vs4_direct_slot: four DIRECT-mode windows, map %d, port base %d"
          % (args.map, base), flush=True)

    procs, logs = [], []
    for k in range(4):
        d = os.path.join(OUT, "p%d" % k)
        os.makedirs(os.path.join(d, "tmp"), exist_ok=True)
        logs.append(os.path.join(d, "run.log"))
        # env_base scrubs every inherited SM64DS_* and sets the quiet+muted
        # rule; pop the level it pins because a VS boot names its own map.
        e = M.env_base(ROOT, d, "p%d" % k)
        e.pop("SM64DS_LEVEL", None)
        e["SM64DS_VS_MAP"] = str(args.map)
        e["SM64DS_WINDOW_SELFTEST"] = str(args.frames)
        e["SM64DS_VS_PROBE"] = "1"
        e["SM64DS_VS_PLAYERS"] = "4"
        e["SM64DS_COMMS_ROLE"] = "parent" if k == 0 else "child"
        e["SM64DS_COMMS_PORT"] = str(base)
        e["SM64DS_COMMS_REPORT"] = "1"
        e["SM64DS_COMMS_INJECT"] = "key=0x%04x" % KEYS[k]
        if k == 0:
            # THE PARENT MUST BE DIRECT TOO, and SM64DS_COMMS_HOST does not do
            # it -- that is a CHILD knob the parent ignores. A parent has no
            # peer to dial; it learns each child from the JOIN. The knob that
            # puts a parent in direct mode is SM64DS_COMMS_BIND_ANY, which
            # comms_loopback.cpp reads as a direct-mode request precisely so a
            # parent is not left in loopback. Without this the parent stays
            # loopback and accepts the children only by an accident of the
            # source-port arithmetic -- exactly the half-working shape rung 1
            # and rung 2 exist to refuse. (Found by running this test: it
            # reported the parent NOT-DIRECT and would not certify the run.)
            e["SM64DS_COMMS_BIND_ANY"] = "1"
        else:
            # DIRECT MODE -- the whole point. A child dials the parent's
            # address, so on_child_packet's assignment arm is reachable.
            e["SM64DS_COMMS_HOST"] = "127.0.0.1:%d" % base
            e["SM64DS_COMMS_SLOT"] = str(k)
        procs.append(M.spawn(EXE, d, e, logs[k]))
        if k < 3:
            import time
            time.sleep(args.stagger)

    rcs = [M.finish(p, 240) for p in procs]
    texts = [M.text(p) for p in logs]
    ok = True

    for k in range(4):
        ok &= M.verdict(rcs[k] == 0, "window %d exited clean | rc=%d %s"
                        % (k, rcs[k], logs[k]))

    # ---- rung 1: the run is ACTUALLY direct mode ---------------------------
    directs = [bool(DIRECT.search(t)) for t in texts]
    ok &= M.verdict(all(directs),
                    "rung1 every window opened in DIRECT mode | %s. A loopback "
                    "fallback would make the adoption arm unreachable and this "
                    "whole test vacuous, so it is asserted first."
                    % (["direct" if d else "NOT-DIRECT" for d in directs],))

    # ---- rung 2: the parent LEARNED all three children over that path ------
    learned = sorted(int(m) for m in LEARNED.findall(texts[0]))
    ok &= M.verdict(learned == [1, 2, 3],
                    "rung2 the parent learned all three children over the "
                    "direct path | learned slots=%s. Proves the assignment arm "
                    "was genuinely exercised rather than skipped." % (learned,))

    # ---- rung 3: NO child adopted a broadcast slot -------------------------
    # THE ASSERTION. On a build with the bit set, a child prints this line and
    # its seated slot collapses onto the announced one.
    adoptions = [(k, ADOPT.findall(t)) for k, t in enumerate(texts)
                 if ADOPT.search(t)]
    ok &= M.verdict(not adoptions,
                    "rung3 NO child adopted a slot from a broadcast ACCEPT | "
                    "%s. This is the bit-clear guarantee: set the assignment "
                    "bit on the roster announce and the windows collapse onto "
                    "one slot." % ("none did" if not adoptions
                                   else "ADOPTED: %s" % adoptions))

    # ---- rung 4: and the seats are distinct 0..3 ---------------------------
    seats = []
    for t in texts:
        m = SEAT.search(t)
        seats.append(int(m.group(2)) if m else -1)
    ok &= M.verdict(seats == [0, 1, 2, 3],
                    "rung4 four distinct seats survived | slots=%s (a collapse "
                    "shows here as a repeated slot)" % (seats,))

    print("\nVERDICT:", "ALL GREEN" if ok else "FAILED")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
