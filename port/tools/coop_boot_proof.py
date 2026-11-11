#!/usr/bin/env python3
"""COOP BOOT PROOF (run mp1 wave 56, lane COOPBOOT1).

Real game processes started the way the launcher starts a coop room (the title
path, SM64DS_COOP*, no SM64DS_LEVEL, and NO SM64DS_SKIP_MENU / SM64DS_BOOT_FILE:
the game forces them), through a local relay copy, quiet and muted through
mp2_proof.env_base, ports from this process id, kill by exact pid. Built on
coop_save_proof.py (same rig: Con, relay, images).

  unit      wire validators and the two barrier rules compiled and run natively
            (cl); controls: a validator that accepts reserved bytes and a start
            rule that never times out must FAIL the same checks
  exist2/4  an existing coop file, 2 / 4 consoles: no menu shown, every console
            released within 1 s of the others; BASE exe control stays on the menu
  hold      one console boots 4 s late: the early ones are held at the start
            barrier, everyone released together
  newfile   a new coop file, 2 consoles: all play the opening, the closing
            barrier holds the early console, everyone released together
  nohost    a guest with no host exits with code 6 after ~15 s
  hostleft  the host is killed mid-level (and, second variant, quits): guests
            show the line and exit with code 7; BASE exe control keeps running
  joiner    a guest that arrives while the opening is running waits at its gate
            until the opening is over, then enters (does not play its own)
  forge     malformed / out-of-role SYNB 0x30-0x3f datagrams: dropped and counted

  python coop_boot_proof.py --exe NEW.exe --base BASE.exe [--root ASSETROOT] [--out DIR] row [row ...]
"""
import argparse
import os
import re
import shutil
import socket
import struct
import subprocess
import sys
import time

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import coop_save_proof as CS  # noqa: E402

say, info = CS.say, CS.info
RES = CS.RES
ARGS = None
CB = re.compile(r"\[coopboot\] t=(\d+) (.*)")


def lines(c):
    return [(int(m.group(1)), m.group(2)) for m in CB.finditer(c.text())]


def first(c, pat):
    for t, s in lines(c):
        if re.search(pat, s):
            return t
    return None


def png(c, name):
    bmp = os.path.join(c.dir, "walk_window_selftest.bmp")
    if not os.path.exists(bmp):
        return None
    d = os.path.join(ARGS.out, "shots")
    os.makedirs(d, exist_ok=True)
    p = os.path.join(d, "%s_%s.png" % (name, c.inst))
    try:
        from PIL import Image
        Image.open(bmp).save(p)
        return p
    except Exception as ex:  # noqa: BLE001
        info("png failed %s" % ex)
        return None


def benv(i, relay, host, sav, extra=None):
    """the launcher's coop env: no SKIP_MENU / BOOT_FILE (the game forces them), no BOOT_FETCH (the game fetches)."""
    e = CS.coop_env(i, relay, host, sav, extra)
    for k in ("SM64DS_SKIP_MENU", "SM64DS_BOOT_FILE", "SM64DS_COOPSAVE_BOOT_FETCH"):
        if k not in (extra or {}):
            e[k] = None
    return e


class Room:
    def __init__(self, row, n, seed, frames, extras=None, gaps=None, exes=None, run=True):
        """n consoles (0 = host). extras[i] = env dict; gaps[i] = seconds before launching console i."""
        self.row = row
        shutil.rmtree(os.path.join(ARGS.out, row), ignore_errors=True)
        self.sav = os.path.join(ARGS.out, row, "hostinstall", "save", "coop", "Pair Room.coopsav")
        if seed == "new":
            CS.write(self.sav, CS.image(CS.TAG_C, None))
        else:
            CS.write(self.sav, CS.image(CS.TAG_C, CS.payload(stars=seed)))
        self.rp, self.rf, self.port, self.rlog = CS.start_relay(row)
        self.relay = ("127.0.0.1:%d" % self.port, "CB%d" % (CS.PID % 1000))
        self.cons = []
        self.n = n
        self.frames = frames
        self.extras = extras or {}
        self.gaps = gaps or {}
        self.exes = exes or {}

    def launch(self, i, frames=None):
        gt = os.path.join(ARGS.out, self.row, "guesttemp%d" % i)
        os.makedirs(gt, exist_ok=True)
        ex = dict(self.extras.get(i, {}))
        if i:
            ex["SM64DS_COOP_TEMP_ROOT"] = gt
        c = CS.Con(self.row, "host" if i == 0 else "guest%d" % i, self.exes.get(i, ARGS.exe),
                   benv(i, self.relay, i == 0, self.sav, ex), frames or self.frames, ARGS.root, ARGS.out)
        self.cons.append(c)
        return c

    def start(self):
        for i in range(self.n):
            g = self.gaps.get(i, 0.5 if i else 0)
            if i and g:
                time.sleep(g)
            # the host may be given a longer run than the guests (host_frames), so a guest that was held
            # back by a barrier still reaches its own last frame (its picture) before the host's run ends
            self.launch(i, self.host_frames if (i == 0 and getattr(self, "host_frames", None)) else None)

    def wait(self, timeout=900, tick=None):
        CS.wait_all(self.cons, timeout, tick)

    def stop(self):
        for c in self.cons:
            if c.poll() is None:
                c.kill()
        CS.stop_relay(self.rp, self.rf)


def released_times(cons):
    out = []
    for c in cons:
        out.append(first(c, r"^RELEASED"))
    return out


# ---------------------------------------------------------------- unit
UNIT = r'''
#include "coop_boot_wire.h"
#include <stdio.h>
using namespace port;
static int fails = 0;
static void row(const char *n, bool ok) { printf("ROW %s %s\n", n, ok ? "ok" : "bad"); if (!ok) ++fails; }
#if BROKEN == 1
// accepts a non-zero reserved byte
static int val(const unsigned char *d, int len) {
    if (len != kCoopBootDatagram || !hub_coop_validate(kHubTagCoopCtl, d, len)) return -1;
    int s = d[6]; return (s >= 0x30 && s <= 0x35) ? s : -1;
}
#else
static int val(const unsigned char *d, int len) { return coop_boot_validate(d, len); }
#endif
#if BROKEN == 2
static bool start_due(unsigned pm, unsigned rm, bool hr, unsigned w) { (void)w; return hr && (pm & ~rm) == 0; }   // never times out
#else
static bool start_due(unsigned pm, unsigned rm, bool hr, unsigned w) { return coop_boot_start_due(pm, rm, hr, w); }
#endif
int main() {
    unsigned char dg[64];
    int n;
    for (int sub = 0x30; sub <= 0x35; ++sub) {
        CoopBootState s = {1, 2, 1};
        n = coop_boot_encode(sub, 7, &s, dg, sizeof dg);
        row(sub == 0x31 ? "state_ok" : (sub == 0x30 ? "ready_ok" : (sub == 0x32 ? "arrived_ok" : (sub == 0x33 ? "close_ok" : (sub == 0x34 ? "hostleft_ok" : "bye_ok")))),
            n == 16 && val(dg, n) == sub);
    }
    CoopBootState s = {1, 2, 1}, d2;
    n = coop_boot_encode(0x31, 9, &s, dg, sizeof dg);
    row("state_roundtrip", coop_boot_decode_state(dg, n, &d2) && d2.go == 1 && d2.phase == 2 && d2.closed == 1);
    // forged shapes: each must be refused
    unsigned char b[64];
    n = coop_boot_encode(0x30, 1, nullptr, b, sizeof b); b[12] = 1;
    row("reserved_byte_refused", val(b, n) == -1);
    n = coop_boot_encode(0x31, 1, &s, b, sizeof b); b[15] = 1;
    row("state_pad_refused", val(b, n) == -1);
    n = coop_boot_encode(0x31, 1, &s, b, sizeof b); b[12] = 2;
    row("state_go2_refused", val(b, n) == -1);
    n = coop_boot_encode(0x31, 1, &s, b, sizeof b); b[13] = 4;
    row("state_phase4_refused", val(b, n) == -1);
    n = coop_boot_encode(0x30, 1, nullptr, b, sizeof b);
    row("short_refused", val(b, n - 1) == -1);
    row("long_refused", val(b, n + 4) == -1);
    n = coop_boot_encode(0x30, 1, nullptr, b, sizeof b); b[4] = 2;
    row("version_refused", val(b, n) == -1);
    n = coop_boot_encode(0x30, 1, nullptr, b, sizeof b); b[7] = 1;
    row("flags_refused", val(b, n) == -1);
    n = coop_boot_encode(0x30, 1, nullptr, b, sizeof b); b[10] = 8;
    row("payload_len_refused", val(b, n) == -1);
    n = coop_boot_encode(0x30, 1, nullptr, b, sizeof b); b[6] = 0x36;
    row("sub36_refused", val(b, n) == -1);
    n = coop_boot_encode(0x30, 1, nullptr, b, sizeof b); b[6] = 0x21;
    row("sub21_not_mine", val(b, n) == -1 && !coop_boot_sub_mine(0x21) && coop_boot_sub_mine(0x3f) && coop_boot_sub_mine(0x30));
    row("null_refused", val(nullptr, 16) == -1);
    // start rule
    row("start_all_ready", start_due(0x6, 0x6, true, 0));
    row("start_waits", !start_due(0x6, 0x2, true, 7999));
    row("start_8s", start_due(0x6, 0x2, true, 8000));
    row("start_needs_host", !start_due(0x6, 0x6, false, 99999));
    row("start_alone", start_due(0, 0, true, 0));
    // close rule
    row("close_all", coop_boot_close_due(0x7, 0x7, true, 0));
    row("close_waits", !coop_boot_close_due(0x7, 0x3, true, 4999));
    row("close_5s", coop_boot_close_due(0x7, 0x3, true, 5000));
    row("close_needs_arrival", !coop_boot_close_due(0x7, 0x0, false, 99999));
    printf("FAILS %d\n", fails);
    return fails ? 1 : 0;
}
'''


def build_unit(broken):
    d = os.path.join(ARGS.out, "unit")
    os.makedirs(d, exist_ok=True)
    src = os.path.join(d, "unit.cpp")
    with open(src, "w") as f:
        f.write(UNIT)
    exe = os.path.join(d, "unit_%d.exe" % broken)
    vc = os.path.expandvars(r"%ProgramFiles(x86)%\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat")
    bat = os.path.join(d, "cl_%d.bat" % broken)
    with open(bat, "w") as f:
        f.write('@echo off\ncall "%s" >nul\ncl /nologo /EHsc /W3 /D_CRT_SECURE_NO_WARNINGS /DBROKEN=%d /I"%s" "%s" /Fe:"%s" /Fo:"%s\\\\" >"%s" 2>&1\n'
                % (vc, broken, os.path.join(REPO, "port", "hal"), src, exe, d, os.path.join(d, "cl_%d.log" % broken)))
    subprocess.call(["cmd", "/c", bat], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return exe


REPO = CS.REPO


def row_unit():
    res = {}
    for broken in (0, 1, 2):
        exe = build_unit(broken)
        if not os.path.exists(exe):
            say(False, "unit: could not compile (broken=%d), see %s" % (broken, os.path.join(ARGS.out, "unit")))
            return
        p = subprocess.run([exe], capture_output=True, text=True)
        res[broken] = (p.returncode, p.stdout)
        with open(os.path.join(ARGS.out, "unit", "unit_%d.out" % broken), "w") as f:
            f.write(p.stdout)
    rows = re.findall(r"ROW (\S+) (ok|bad)", res[0][1])
    bad = [n for n, v in rows if v == "bad"]
    say(res[0][0] == 0 and len(rows) >= 27 and not bad, "unit: wire validators + both barrier rules, %d checks, %d bad %s" % (len(rows), len(bad), bad))
    b1 = [n for n, v in re.findall(r"ROW (\S+) (ok|bad)", res[1][1]) if v == "bad"]
    b2 = [n for n, v in re.findall(r"ROW (\S+) (ok|bad)", res[2][1]) if v == "bad"]
    say(res[1][0] != 0 and ("reserved_byte_refused" in b1 or "state_pad_refused" in b1), "unit MUST-FAIL: a validator that accepts reserved bytes fails %s" % b1)
    say(res[2][0] != 0 and "start_8s" in b2, "unit MUST-FAIL: a start rule with no 8 s timeout fails %s" % b2)


# ---------------------------------------------------------------- existing file
def row_exist(n):
    row = "exist%d" % n
    frames = 1400
    gaps = {i: 0.2 for i in range(1, n)}
    rm = Room(row, n, [(0, 0)], frames, gaps=gaps)
    rm.start()
    rm.wait(900)
    rm.stop()
    cons = rm.cons
    rel = released_times(cons)
    info("%s exit codes %s, RELEASED unix ms %s" % (row, [c.rc for c in cons], rel))
    ok_rel = all(r is not None for r in rel)
    spread = (max(rel) - min(rel)) if ok_rel else None
    say(ok_rel and spread < 1000, "%s: all %d consoles released (first tick of the file) within %s ms of each other (limit 1000)" % (row, n, spread))
    say(all(c.rc == 0 for c in cons), "%s: every console ran to its end, rc %s" % (row, [c.rc for c in cons]))
    skip = [("[skip-menu]" in c.text(), "pressed file A" in c.text()) for c in cons]
    say(all(a and b for a, b in skip), "%s: no menu shown: every console's own title skip chose the file select and pressed file A (%s) with no SM64DS_SKIP_MENU / BOOT_FILE in its launch env" % (row, skip))
    envs = [open(os.path.join(c.dir, "env.txt")).read() for c in cons]
    say(not any("SM64DS_SKIP_MENU" in e or "SM64DS_BOOT_FILE" in e for e in envs), "%s: the launch env really lacked SKIP_MENU and BOOT_FILE (env.txt of every console)" % row)
    cl = [(c.inst, [s for t, s in lines(c) if s.startswith(("HOLD", "RELEASED", "GO", "READY from", "host ", "guest "))][:8]) for c in cons]
    for inst, ls in cl:
        info("%s %s: %s" % (row, inst, ls))
    pics = [png(c, row) for c in cons]
    info("%s pictures (last frame of each console, after the release): %s" % (row, [os.path.basename(p) if p else None for p in pics]))
    say(all(pics), "%s: a picture from each console exists (shots/)" % row)
    return rm


def row_exist_base():
    row = "exist_base"
    rm = Room(row, 1, [(0, 0)], 900, exes={0: ARGS.base})
    rm.start()
    rm.wait(600)
    rm.stop()
    c = rm.cons[0]
    t = c.text()
    say("[skip-menu]" not in t and "pressed file A" not in t and "[coopboot]" not in t,
        "exist MUST-FAIL: the BASE exe with the same coop env (no SKIP_MENU / BOOT_FILE) does not skip the menu or press the file (it stays on the title / menu); no coopboot lines")
    png(c, row)


# ---------------------------------------------------------------- hold (late console)
def row_hold():
    row = "hold"
    rm = Room(row, 3, [(0, 0)], 1600, extras={2: {"SM64DS_COOPBOOT_TEST_DELAY_MS": "4000"}}, gaps={1: 0.5, 2: 0.5})
    rm.start()
    rm.wait(900)
    rm.stop()
    cons = rm.cons
    rel = released_times(cons)
    hold_t = [first(c, r"^HOLD: start barrier") for c in cons]
    ready_t = [first(c, r"first frame") for c in cons]
    info("%s first frame times %s, hold lines %s, released %s, rcs %s" % (row, ready_t, hold_t, rel, [c.rc for c in cons]))
    late = ready_t[2]
    ok = all(r is not None for r in rel) and (max(rel) - min(rel)) < 1000
    say(ok, "%s: released within %s ms of each other" % (row, (max(rel) - min(rel)) if ok else "?"))
    say(hold_t[0] is not None and hold_t[1] is not None and late is not None and hold_t[0] < late - 1500 and min(rel) >= late - 300,
        "%s: the early consoles were held at the start barrier (hold at %s / %s) until the 4 s late console reached its first frame (%s); nobody released before it (first release %s)"
        % (row, hold_t[0], hold_t[1], late, min(rel) if rel else None))
    host_go = first(cons[0], r"^GO:")
    say(host_go is not None and "timed out" not in [s for t, s in lines(cons[0]) if s.startswith("GO:")][0], "%s: the host sent GO because everyone was READY, not on the 8 s timeout" % row)
    return rm


# ---------------------------------------------------------------- nohost
def row_nohost():
    row = "nohost"
    shutil.rmtree(os.path.join(ARGS.out, row), ignore_errors=True)
    rp, rf, port, rlog = CS.start_relay(row)
    relay = ("127.0.0.1:%d" % port, "CN%d" % (CS.PID % 1000))
    gt = os.path.join(ARGS.out, row, "gt")
    os.makedirs(gt, exist_ok=True)
    c = CS.Con(row, "guest", ARGS.exe, benv(1, relay, False, None, {"SM64DS_COOP_TEMP_ROOT": gt}), 400, ARGS.root, ARGS.out)
    CS.wait_all([c], 120)
    t = c.text()
    t_begin = first(c, r"^coop boot: role=guest")
    t_exit = first(c, r"exiting with code 6")
    dt = (t_exit - t_begin) if (t_begin and t_exit) else None
    info("%s: rc %s, begin %s exit %s => %s ms; stderr line: %s" % (row, c.rc, t_begin, t_exit, dt, re.findall(r"Could not reach the host.*", t)[:1]))
    say(c.rc == 6 and dt is not None and 14900 <= dt <= 15600 and "Could not reach the host" in t,
        "%s: a guest with no host exits with code 6 %s ms after its gate began (limit 15600) and says 'Could not reach the host'" % (row, dt))
    say(c.t_end - c.t0 < 30, "%s: process wall time %.1f s" % (row, c.t_end - c.t0))
    cb = CS.Con(row, "guest_base", ARGS.base, benv(1, relay, False, None, {"SM64DS_COOP_TEMP_ROOT": gt}), 400, ARGS.root, ARGS.out)
    CS.wait_all([cb], 120)
    CS.stop_relay(rp, rf)
    say(cb.rc != 6 and "Could not reach the host" not in cb.text(), "%s MUST-FAIL: the BASE exe with the same env (no host) does not exit 6, rc %s" % (row, cb.rc))


# ---------------------------------------------------------------- new file
def row_newfile(n=2):
    row = "newfile"
    frames = 5200
    rm = Room(row, n, "new", frames, extras={1: {"SM64DS_COOPBOOT_TEST_SLOW_MS": "3000", "SM64DS_INTRO_WATCH": "1"}, 0: {"SM64DS_INTRO_WATCH": "1"}})
    rm.host_frames = frames + 1000
    rm.start()
    rm.wait(1500)
    rm.stop()
    cons = rm.cons
    info("%s rcs %s" % (row, [c.rc for c in cons]))
    play = [first(c, r"the opening is playing here") for c in cons]
    arr = [first(c, r"^ARRIVED at") for c in cons]
    rel = [first(c, r"^RELEASED") for c in cons]
    # the second RELEASED line (after the end barrier)
    rel2 = []
    for c in cons:
        rr = [t for t, s in lines(c) if s.startswith("RELEASED")]
        rel2.append(rr[1] if len(rr) > 1 else None)
    holds = [[(t, s) for t, s in lines(c) if s.startswith("HOLD")] for c in cons]
    info("%s opening playing %s, arrived %s, start release %s, end release %s" % (row, play, arr, rel, rel2))
    for c, h in zip(cons, holds):
        info("%s %s hold lines %s" % (row, c.inst, h))
    say(all(play), "%s: every console played the opening (the new file)" % row)
    ok2 = all(x is not None for x in rel2)
    say(ok2 and max(rel2) - min(rel2) < 1000, "%s: everyone proceeded into the grounds together: end release spread %s ms" % (row, (max(rel2) - min(rel2)) if ok2 else "?"))
    gap = (arr[1] - arr[0]) if (arr[0] and arr[1]) else None
    say(arr[0] is not None and arr[1] is not None and abs(gap) >= 1000 and ((arr[0] < arr[1] and any("opening end" in s for t, s in holds[0])) or (arr[1] < arr[0] and any("opening end" in s for t, s in holds[1]))),
        "%s: the early console was held at the closing barrier (arrived %s ms apart; hold lines %s)" % (row, gap, [len(h) for h in holds]))
    host_close = [s for t, s in lines(cons[0]) if s.startswith("CLOSE:")]
    say(bool(host_close), "%s: host CLOSE line: %s" % (row, host_close[:1]))
    t0 = cons[0].text()
    t1 = cons[1].text()
    e0 = re.findall(r"(?m)^\s*\[intro\] flags2 bit 7 .*$", t0)
    e1 = re.findall(r"(?m)^\s*\[intro\] flags2 bit 7 .*$", t1)
    info("%s opening-seen edges host %s guest %s" % (row, e0[:2], e1[:2]))
    say(e0 == e1 and len(e0) >= 1, "%s: both consoles saw the opening-seen bit edge at the same ROM frame" % row)
    for c in cons:
        png(c, row)
    return rm


# ---------------------------------------------------------------- host left
def row_hostleft():
    for variant in ("kill", "quit"):
        row = "hostleft_" + variant
        frames_h = 3000 if variant == "kill" else 700
        rm = Room(row, 3, [(0, 0)], 3000, gaps={1: 0.5, 2: 0.5},
                  extras={1: {"SM64DS_COOPBOOT_POPUP_SHOT": os.path.join(ARGS.out, row, "popup_guest1.bmp")},
                          2: {"SM64DS_COOPBOOT_POPUP_SHOT": os.path.join(ARGS.out, row, "popup_guest2.bmp")}})
        rm.launch(0, frames_h)
        time.sleep(0.5)
        rm.launch(1)
        time.sleep(0.5)
        rm.launch(2)
        cons = rm.cons
        t_kill = None
        dl = time.time() + 900
        while time.time() < dl:
            if variant == "kill" and t_kill is None and all("ticked frame 120" in c.text() for c in cons):
                t_kill = time.time()
                cons[0].kill()
                info("%s: killed the host (pid %d) at %.1f s into the run" % (row, cons[0].pid, t_kill - cons[0].t0))
            if all(c.poll() is not None for c in cons[1:]):
                break
            for c in cons:
                c.poll()
            time.sleep(0.1)
        for c in cons:
            c.poll()
        gt_end = [c.t_end for c in cons[1:]]
        rm.stop()
        g = cons[1:]
        for c in g:
            L = [(t, s) for t, s in lines(c) if "host" in s.lower() and ("left" in s.lower() or "gone" in s.lower())]
            info("%s %s rc %s: %s" % (row, c.inst, c.rc, L[:4]))
        say(all(c.rc == 7 for c in g), "%s: every guest exited with the host-left code 7 (rcs %s)" % (row, [c.rc for c in g]))
        say(all("The host left. Room closed." in c.text() for c in g), "%s: every guest said 'The host left. Room closed.'" % row)
        if variant == "kill" and t_kill:
            dts = [round(e - t_kill, 1) for e in gt_end if e]
            say(all(d is not None and 5.0 <= d <= 9.0 for d in dts), "%s: guests left %s s after the host was killed (presence timeout ~3 s + the 3 s line)" % (row, dts))
        else:
            # clean quit: HOST_LEFT message, no waiting for the presence timeout
            t_h = cons[0].t_end
            dts = [round(e - t_h, 1) for e in gt_end if e]
            hl = [first(c, r"HOST_LEFT heard") for c in g]
            say(all(h is not None for h in hl) and all(d <= 5.5 for d in dts), "%s: HOST_LEFT message heard by every guest (%s); guests left %s s after the host exited" % (row, hl, dts))
        line_lens = []
        for c in g:
            tt = first(c, r"The host left")
            line_lens.append(round((c.t_end - tt / 1000.0), 1) if tt else None)
        say(all(x is not None and 2.5 <= x <= 4.0 for x in line_lens), "%s: the line stayed up ~3 s before exit: %s s" % (row, line_lens))
        for c in g:
            b = os.path.join(ARGS.out, row, "popup_%s.bmp" % c.inst)
            if os.path.exists(b):
                try:
                    from PIL import Image
                    d = os.path.join(ARGS.out, "shots")
                    os.makedirs(d, exist_ok=True)
                    Image.open(b).save(os.path.join(d, "%s_%s_line.png" % (row, c.inst)))
                    info("%s picture of the line saved: shots/%s_%s_line.png" % (row, row, c.inst))
                except Exception as ex:  # noqa: BLE001
                    info("png failed %s" % ex)
            else:
                info("%s: no popup picture for %s" % (row, c.inst))
        # MUST-FAIL control on the base exe
        if variant == "kill":
            rb = Room(row + "_base", 2, [(0, 0)], 1000, gaps={1: 0.5}, exes={0: ARGS.base, 1: ARGS.base})
            rb.launch(0, 3000)
            time.sleep(0.5)
            rb.launch(1)
            t0 = time.time()
            dl = time.time() + 400
            killed = False
            while time.time() < dl:
                if not killed and time.time() - t0 > 90:
                    rb.cons[0].kill()
                    killed = True
                    tk = time.time()
                if killed and time.time() - tk > 25:
                    break
                for c in rb.cons:
                    c.poll()
                if rb.cons[1].rc is not None:
                    break
                time.sleep(0.2)
            g1 = rb.cons[1]
            still = g1.poll() is None
            rcg = g1.rc
            rb.stop()
            say(still or rcg != 7, "%s MUST-FAIL: with the BASE exe the guest does not leave with code 7 after the host dies (rc %s, still running %s)" % (row, rcg, still))


# ---------------------------------------------------------------- joiner during the opening
def row_joiner():
    row = "joiner"
    rm = Room(row, 2, "new", 5200, gaps={1: 0.5})
    # host first, guest 1 normally; a THIRD console arrives mid-opening
    rm.n = 2
    rm.start()
    cons = rm.cons
    c3 = None
    dl = time.time() + 1500
    t_join = None
    while time.time() < dl:
        for c in cons:
            c.poll()
        if c3 is None and "the opening is playing here" in cons[0].text() and first(cons[0], r"^RELEASED") and cons[0].poll() is None:
            time.sleep(6)
            gt = os.path.join(ARGS.out, row, "guesttemp2")
            os.makedirs(gt, exist_ok=True)
            c3 = CS.Con(row, "joiner", ARGS.exe, benv(2, rm.relay, False, None, {"SM64DS_COOP_TEMP_ROOT": gt}), 4200, ARGS.root, ARGS.out)
            cons.append(c3)
            t_join = time.time()
        if c3 and all(c.poll() is not None for c in cons):
            break
        time.sleep(0.2)
    rm.stop()
    if not c3:
        say(False, "%s: the host never started its opening" % row)
        return
    L = lines(c3)
    wait = [s for t, s in L if "joined during the opening" in s]
    enter = first(c3, r"the opening ended; entering")
    first_frame = first(c3, r"first frame")
    own = "the opening is playing here" in c3.text()
    info("%s joiner lines %s" % (row, [s for t, s in L][:10]))
    say(bool(wait) and enter is not None, "%s: the joiner arrived mid-opening and waited at its gate (%d wait lines), then entered when the host's opening was over" % (row, len(wait)))
    say(not own, "%s: the joiner did NOT play its own opening" % row)
    # the joiner started late, so the host's frame cap ends first: the joiner then rightly exits with the host-left code (7)
    host_left = "The host left. Room closed." in c3.text()
    say(c3.rc == 0 or (c3.rc == 7 and host_left and enter is not None), "%s: joiner rc %s (0, or 7 after entering once the host's run ended)" % (row, c3.rc))


# ---------------------------------------------------------------- forged datagrams
def row_forge():
    """a raw UDP client joins the hub as a guest-shaped seat is not possible without the HELLO; the wire rules are
    covered natively by 'unit'. This row runs the real consoles with SM64DS_COOPBOOT_FORGE=<mode>@<ms> (a console
    sends ONE hand-made datagram) and reads the receivers' drop lines and that nothing moved."""
    row = "forge"
    rm = Room(row, 2, [(0, 0)], 1200, gaps={1: 0.5},
              extras={0: {"SM64DS_COOPBOOT_FORGE": "ready_from_host,badlen,state_pad"},
                      1: {"SM64DS_COOPBOOT_FORGE": "badlen,reserved,state_from_guest,unknown_subtype,hostleft_from_guest"}})
    rm.start()
    rm.wait(900)
    rm.stop()
    h, g = rm.cons
    dh = re.findall(r"\[coopboot\] t=\d+ DROP.*", h.text())
    dg = re.findall(r"\[coopboot\] t=\d+ DROP.*", g.text())
    info("%s host drop lines: %s" % (row, dh[:8]))
    info("%s guest drop lines: %s" % (row, dg[:8]))
    # host got 5 forged (badlen, reserved, state_from_guest, unknown_subtype, hostleft_from_guest); guest got 3
    say(len(dh) >= 5 and len(dg) >= 3, "%s: every forged datagram was dropped and said so (host %d of 5, guest %d of 3)" % (row, len(dh), len(dg)))
    say(h.rc == 0 and g.rc == 0, "%s: nothing happened because of them: neither console left early (rc %s/%s) and the guest did not take the forged host-left" % (row, h.rc, g.rc))
    say("The host left" not in g.text(), "%s: a forged HOST_LEFT from a guest seat did not close the room" % row)


def main():
    global ARGS
    ap = argparse.ArgumentParser()
    ap.add_argument("--exe", required=True)
    ap.add_argument("--base", required=True)
    ap.add_argument("--root", default=REPO)
    ap.add_argument("--out", default=os.path.join(REPO, "build", "coop_boot_proof"))
    ap.add_argument("rows", nargs="+")
    ARGS = ap.parse_args()
    ARGS.exe = os.path.abspath(ARGS.exe)
    ARGS.base = os.path.abspath(ARGS.base)
    CS.ARGS = ARGS
    os.makedirs(ARGS.out, exist_ok=True)
    table = {"unit": row_unit, "exist2": lambda: row_exist(2), "exist4": lambda: row_exist(4), "exist_base": row_exist_base,
             "hold": row_hold, "nohost": row_nohost, "newfile": row_newfile, "hostleft": row_hostleft,
             "joiner": row_joiner, "forge": row_forge}
    for r in ARGS.rows:
        table[r]()
    bad = [r for r in RES if r.startswith("FAIL")]
    print("\n%d PASS %d FAIL" % (len([r for r in RES if r.startswith("PASS")]), len(bad)))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
