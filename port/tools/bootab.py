"""Headless, silent boot sweep, same knobs for every binary, so builds compare.
usage: python bootab.py <walk_window.exe> <outdir> [frames] [budget_s]
       [levels=2,4,5 | scenes=4,5 | levels=all | scenes=all] [idle=1] [aspect=<ratio>]
       [smooth=<0..3>] [texfilter=<0..2>] [aa=<0..1>] [scale=<0..4>]
       [present=<0|1>] [presentfilter=<0..2>] [vsync=<0|1>]
       [presentdev=warp|hw] [presentoff=<0|1>] [workers=<n>]
workers=<n> (run link100, lane SWEEPPAR1) runs up to n rows at a time instead of one
after another. Default 1 is this tool's whole history, byte for byte: nobody who does
not name workers= (or export SM64DS_SWEEP_WORKERS, an equivalent default) sees any
difference at all -- same one exe, same one directory, same output. n>1 gives each
worker its OWN COPY of the exe in its own scratch directory under this script's own
tree's tmp/sweep-workers/ (never under the tree the exe under test lives in, which can
be someone else's read-only build), because crash.txt/exit.txt/startup_error.txt are
EXE-RELATIVE, not cwd-relative, and two rows sharing a folder would have the second
faulting row overwrite the first's report -- battery.py's level_workers/worker_dirs
design (see the comment above run link100 lane SLOT's code in battery.py), reused
here rather than invented fresh. Rows are submitted and reported in the SAME ORDER a
serial run would visit them regardless of which one finishes first, so the sweep.tsv
and the SUMMARY line are identical in shape and order to a workers=1 run of the same
filter, just faster. The ceiling is derived from the box's own CPU count and capped
well under it, since this box runs many other lanes at once; see the WORKER WIDTH
comment further down for the measured number. Every arm below grades PASS/FAIL from
a return code and the log text, never from a rate or a wall-clock time, so none of
them needed to be forced back to serial width.
The SM64DS_TEST_LOCK* variables are passed through (every other inherited SM64DS_* is dropped),
so exporting SM64DS_TEST_LOCK=1 SM64DS_TEST_LOCK_PATH=C:/tmp/sm64ds-test-slot/slot.lock
SM64DS_TEST_LOCK_TIMEOUT=10800 before running makes every row take the machine-wide test slot.
Levels: SM64DS_LEVEL=<id> SM64DS_WINDOW_SELFTEST=<frames> (the battery's own level path)
Scenes: SM64DS_SCENE=<id> SM64DS_SCENE_FRAMES=<frames>
Always: SM64DS_FAULTS_FATAL=1 SM64DS_NO_FOCUS=1 SM64DS_VOLUME=0, no SCENE_WINDOW,
idle=1 adds SM64DS_SELFTEST_IDLE=1 to the LEVEL rows, which walk_window.cpp uses to
leave dz at 0 so the selftest neither holds forward nor hops at frame 30: the level
boots and then sits still. Scene rows have no selftest and are unaffected.
smooth=<0..3> sets SM64DS_SMOOTH_MODELS on EVERY row, level and scene alike, which
is the "SmoothModels" setting (run hd1, lane MDL). Same shape and same reason as
aspect= below: the environment scrub drops an inherited one, so a sweep that does
not name it is byte-identical to one from before this argument existed.
texfilter=<0..2> and aa=<0..1> set SM64DS_TEXTURE_FILTER and SM64DS_ANTI_ALIASING the
same way on every row (run hd2, lane PIC): the "TextureFilter" and "AntiAliasing"
settings. Same shape and same reason as smooth= above; unnamed, a sweep is byte for
byte the one this tool ran before they existed. scale=<0..4> is the same again
for SM64DS_RENDER_SCALE ("RenderScale", run hd1).
present=<0|1>, presentfilter=<0..2> and vsync=<0|1> are the same shape once more,
for SM64DS_PRESENT_BACKEND, SM64DS_PRESENT_FILTER_D3D and SM64DS_VSYNC (the
"PresentBackend", "PresentFilter" and "VSync" settings, run hd2 lane GPU1): which
path hands the finished picture to the screen. presentdev=warp|hw pins the device
(SM64DS_PRESENT_DEVICE) and presentoff=1 puts the backend in its offscreen proof
mode (SM64DS_PRESENT_OFFSCREEN), which draws the identical upload, quad and filter
into a target of its own and needs no window -- which is what makes it usable from
a headless sweep at all. All five are inert unless named.
minimap=<size> turns the improved minimap ON at that size on every row (the
"ImprovedMinimap" and "MinimapScale" settings), and minimapdir=<folder> points its
panel artwork at a folder. Same shape and same reason as smooth= above, with one
extra: the option's getter is pinned off under SM64DS_WINDOW_SELFTEST so a stray
settings.json can never move a recorded baseline, which makes the environment the
only channel a sweep has to it.
aspect=<ratio> sets SM64DS_ASPECT to that ratio on EVERY row, level and scene alike,
so one sweep boots the whole table at one presentation width. It is the only way to
reach the wide path from here: the environment scrub above drops an inherited
SM64DS_ASPECT with the rest of the SM64DS_* block, so a sweep that did not name it
is always the native 4:3 picture. 0 (or no aspect= at all) is that native sentinel;
the game clamps the ratio itself (port/hal/host_settings.cpp aspect_sanitise), so a
value this tool passes through unchanged is still sanitised before it sizes a buffer.
CREATE_NO_WINDOW + SW_SHOWMINNOACTIVE, every inherited SM64DS_* dropped.
Failing rows keep crash.txt + exit.txt + stdout tail under <outdir>/<kind><id>/.

warpin=1 and reentry=1 change HOW a LEVEL row is entered, and nothing else. Every
other arm of this tool boots the level directly (SM64DS_LEVEL=<id>), which is not
how the game is played: a course is entered by a LEVEL CHANGE out of the castle,
and state that is right on a direct boot can be wrong after a change (wave 13's
texture, minimap and star-select bugs, and lane SEAT14B's Jolly Roger Bay fault,
were all of that family, invisible to every direct-boot gate).
  warpin=1  boots castle grounds (SM64DS_LEVEL=1) and warps into the row's level
            at frame 600 with SM64DS_WARP_SEQ, selftest 1500. Row 1 is entered
            from level 6 instead, since it cannot warp into itself.
  reentry=1 boots the row's level, warps OUT to castle grounds at frame 400 and
            back IN at frame 800 (SM64DS_WARP_SEQ=1@400,<id>@800), selftest 1400,
            which is the leave-and-come-back shape SEAT14B faulted on. Row 1
            leaves to level 6 and back.
Both drive the game's own LoadLevelNoReturn through walk_window.cpp's warp-seq
block, so the teardown, the level-change poll and the boot half all run for real.
press=<spec> is the scripted pad both arms hand to SM64DS_PROBE_INPUT, default
200:A: entering a COURSE from the castle takes the painting route, so
hal/level_change.cpp runs the star select inline between the teardown and the
boot half (SM64DS_STARSEL_PAINTING, default on), and without an A press nothing
chooses an act and the interlude only ends at its 1800-frame backstop. press=none
drops the variable, which is the straight-in A/B of the same row.
Their FRAMES (argv[3]) is fixed by the arm, so a warp row is the same length on
every binary; the SCENE rows of a sweep are untouched by both.

exit=<kind> changes how a LEVEL row is LEFT, which warpin/reentry do not cover:
they prove a course's boot after a change, and every one of them ends with the
course still standing. Nothing else in this run has ever run a course's exit.
  exit=void   SM64DS_VOID_DROP=200 -- below the death plane, so the cartridge's
              own func_ov002_020c5d60 -> ST_DEAD_PIT -> HitDeathPlane -> the
              level change back to the hub with the death reason (2)
  exit=star   SM64DS_STAR_DROP=#0,200 plus A presses -- stand on the level's
              first PowerStar, let its collision run the star-get sequence and
              the course-clear exit (reason 1). Also drives HOST_PAD (A then
              DPAD_LEFT) after the arrival and fails the row if [exitpos]
              shows the player never moved.
  exit=pause  START then the fourth pause button on the touch screen -- exit
              course (reason 0)
All three set selftest 1200 and SM64DS_EXIT_WATCH=1, and a row is FAIL unless
the run carries a "[lvl] change:" line WITH THE REASON THAT ARM ASKED FOR (void
2, star 1, pause 0), which is printed in the note. Both halves are needed: an
exit that quietly does nothing exits 0 with the level still up, and a selftest
that walks a course for 1200 frames falls off plenty of them on its own, so a
change with the wrong reason measured the fall and not the arm. Scene rows are
untouched.

entrance=<n|all> changes WHICH ENTRANCE RECORD a LEVEL row is entered at, which
no other arm of this tool has ever moved: every row above enters at record 0.
Until port/hal/level_boot.cpp was fixed (the entrance the change asked for, not
always record 0) the port could not enter a level at any other record at all, so
every mode in the cartridge's entry-mode table except the two that record 0
happens to carry is code no gate in this run has executed.
  entrance=<n>   every level row boots with SM64DS_ENTRANCE=<n>, i.e. record n
  entrance=all   each level row is EXPANDED into one row per entrance record the
                 cartridge gives that level, read here out of the ROM itself
SM64DS_ENTRANCE seats data_0209f268, the PENDING entrance, and Stage's own
`data_0209f264 = data_0209f268` then latches it, so a direct boot selects the
record exactly the way a level change does. Rows carry the record in their own
column and in their output directory name (level<id>_e<rec>), and a level row's
note carries the run's `selftest: N frames, pos=(x, y, z)` line, because an
entrance whose mode leaves the player stuck in his arrival animation exits 0 and
is still a finding: its position barely moves under the held-forward walk while
record 0's travels. Scene rows are untouched by the arm, and a sweep that does
not name it is byte-identical to one from before the arm existed.
"""
import os, re, sys, time, shutil, subprocess, queue, threading, signal
from concurrent.futures import ThreadPoolExecutor
EXE = os.path.abspath(sys.argv[1]); OUT = os.path.abspath(sys.argv[2])
FRAMES = sys.argv[3] if len(sys.argv) > 3 else "600"
BUDGET = int(sys.argv[4]) if len(sys.argv) > 4 else 90
EXEDIR = os.path.dirname(EXE); ROOT = os.path.dirname(os.path.dirname(EXEDIR))
sys.path.insert(0, os.path.join(ROOT, "port", "tools"))
import battery as B
LEVELS = B.mounted_levels(ROOT); SCENES = B.hosted_scenes(ROOT)
os.makedirs(OUT, exist_ok=True)
SI = subprocess.STARTUPINFO(); SI.dwFlags |= subprocess.STARTF_USESHOWWINDOW; SI.wShowWindow = 7
NOCON = getattr(subprocess, "CREATE_NO_WINDOW", 0)
base_env = {k: v for k, v in os.environ.items() if not k.startswith("SM64DS_") or k.startswith("SM64DS_TEST_LOCK")}
FILTER = sys.argv[5] if len(sys.argv) > 5 else ""
IDLE = any(a == "idle=1" for a in sys.argv[5:])
ASPECT = ""
for a in sys.argv[5:]:
    if a.startswith("aspect="): ASPECT = a[7:]
SMOOTH = ""
for a in sys.argv[5:]:
    if a.startswith("smooth="): SMOOTH = a[7:]
TEXFILTER = ""
for a in sys.argv[5:]:
    if a.startswith("texfilter="): TEXFILTER = a[10:]
AA = ""
for a in sys.argv[5:]:
    if a.startswith("aa="): AA = a[3:]
SCALE = ""
for a in sys.argv[5:]:
    if a.startswith("scale="): SCALE = a[6:]
# run hd2, lane GPU1: the present backend, its two settings and the two test
# knobs a headless sweep needs to reach the graphics-card path with no window.
PRESENT = ""
for a in sys.argv[5:]:
    if a.startswith("present="): PRESENT = a[8:]
PRESENTFILTER = ""
for a in sys.argv[5:]:
    if a.startswith("presentfilter="): PRESENTFILTER = a[14:]
VSYNC = ""
for a in sys.argv[5:]:
    if a.startswith("vsync="): VSYNC = a[6:]
PRESENTDEV = ""
for a in sys.argv[5:]:
    if a.startswith("presentdev="): PRESENTDEV = a[11:]
PRESENTOFF = ""
for a in sys.argv[5:]:
    if a.startswith("presentoff="): PRESENTOFF = a[11:]
MINIMAP = ""
for a in sys.argv[5:]:
    if a.startswith("minimap="): MINIMAP = a[8:]
MINIMAPDIR = ""
for a in sys.argv[5:]:
    if a.startswith("minimapdir="): MINIMAPDIR = a[11:]
WARPIN = any(a == "warpin=1" for a in sys.argv[5:])
REENTRY = any(a == "reentry=1" for a in sys.argv[5:])
PRESS = "200:A"
for a in sys.argv[5:]:
    if a.startswith("press="): PRESS = a[6:]
EXIT = ""
for a in sys.argv[5:]:
    if a.startswith("exit="): EXIT = a[5:]
ENTRANCE = ""
for a in sys.argv[5:]:
    if a.startswith("entrance="): ENTRANCE = a[9:]

# WORKER WIDTH (run link100, lane SWEEPPAR1). Default 1 is today's behaviour,
# byte for byte: no workers= argument and no SM64DS_SWEEP_WORKERS in the
# environment runs the exact old serial loop below, one exe, no copies, no
# new directories. Set either one to run rows N at a time. The ceiling is not
# battery.py's fixed MAX_LEVEL_WORKERS=8 -- this box runs however many other
# orchestration lanes, fixers and builds alongside one sweep, not just this
# tool alone -- so it is derived from the box's own CPU count and then kept
# well under it. Measured on this box (2026-09-20): os.cpu_count() = 12
# (PowerShell Get-CimInstance Win32_Processor: 6 physical cores, 12 logical
# with hyperthreading). cpu_count // 3 leaves two-thirds of the logical count
# free for everything else sharing the machine tonight; on this box that is
# 4, which is also the width every PROOF in the commit was measured at. The
# min(8, ...) keeps it from ever exceeding battery.py's own proven width on a
# bigger box.
def _workers_env_default():
    v = os.environ.get("SM64DS_SWEEP_WORKERS", "")
    try:
        return int(str(v).strip())
    except ValueError:
        return 1
MAX_SWEEP_WORKERS = max(1, min(8, (os.cpu_count() or 12) // 3))
WORKERS = _workers_env_default()
for a in sys.argv[5:]:
    if a.startswith("workers="): WORKERS = int(a[8:])
WORKERS = max(1, min(WORKERS, MAX_SWEEP_WORKERS))

# the row filter is positional but the flags are not, so a run that passes only a
# flag must not have that flag read as a filter (it would then match no prefix and
# sweep everything by accident)
if FILTER in ("idle=1", "warpin=1", "reentry=1") or FILTER.startswith(("aspect=", "press=", "exit=", "entrance=", "smooth=", "texfilter=", "aa=", "scale=", "present=", "presentfilter=", "vsync=", "presentdev=", "presentoff=", "workers=", "minimap=", "minimapdir=")): FILTER = ""
if FILTER.startswith("levels="):
    sel = FILTER[7:]; SCENES = ()
    if sel != "all": LEVELS = tuple(i for i in LEVELS if str(i) in sel.split(","))
elif FILTER.startswith("scenes="):
    sel = FILTER[7:]; LEVELS = ()
    if sel != "all": SCENES = tuple(i for i in SCENES if str(i) in sel.split(","))
def entrance_counts(root):
    """How many entrance records each level has, read off the cartridge.

    The walk is the ROM's own and the same one port/tools/ov_places.py uses for
    object tables: data_020758c8[level] is the level's object overlay id,
    data_02092208[level] its LVL_Overlay; the LVL_Overlay's misc table is at +4
    and its sub-table array at +0x10 with the count at +0x14; every 8-byte
    sub-table entry is {descriptor, count, pad2, records} and the descriptor's
    low five bits are the sub-loader index, of which 1 is LOADER_ENTRANCE.
    """
    import re, struct, pathlib
    rt = pathlib.Path(root)
    rng = {}
    for p in sorted((rt / "config/arm9/overlays").glob("ov*/delinks.txt")):
        ov = int(re.search(r"ov(\d+)", str(p)).group(1))
        t = p.read_text()
        s = [int(x, 16) for x in re.findall(r"start:(0x[0-9a-fA-F]+)", t)]
        rng[ov] = min(s)
    arm9 = (rt / "extracted/arm9_dec.bin").read_bytes()
    a32 = lambda x: struct.unpack_from("<I", arm9, x - 0x02004000)[0]
    out = {}
    for lvl in range(52):
        ovid = a32(0x020758C8 + lvl * 4)
        lo = a32(0x02092208 + lvl * 4)
        f = rt / ("extracted/overlays/overlay_%04d.bin" % ovid)
        if ovid not in rng or not f.exists():
            continue
        base = rng[ovid]; d = f.read_bytes()
        has = lambda a, n=1: base <= a and a + n <= base + len(d)
        u8 = lambda a: d[a - base]
        u16 = lambda a: struct.unpack_from("<H", d, a - base)[0]
        u32 = lambda a: struct.unpack_from("<I", d, a - base)[0]
        if not has(lo, 0x18):
            continue
        tables = [u32(lo + 4)]
        subs = u32(lo + 0x10); nsub = u8(lo + 0x14)
        if has(subs, nsub * 0xC):
            tables += [u32(subs + s * 0xC) for s in range(nsub)]
        n_ent = 0
        for t in tables:
            if not t or not has(t, 8):
                continue
            n = u16(t); ents = u32(t + 4)
            if not has(ents, n * 8):
                continue
            for j in range(n):
                e = ents + j * 8
                if (u8(e) & 0x1F) != 1:
                    continue
                cnt = u8(e + 1)
                if has(u32(e + 4), cnt * 0x10):
                    n_ent += cnt
        if n_ent:
            out[lvl] = n_ent
    return out


ENTCOUNT = entrance_counts(ROOT) if ENTRANCE == "all" else {}
ART = ("crash.txt", "exit.txt")

# THE PROCESS TABLE. Every child this script starts is tracked here for its
# whole lifetime, so a Ctrl-C (or any other reason to stop early) kills only
# what THIS process spawned, never a game window someone else started (a
# walk_window.exe this script did not launch is not this script's to touch).
_proc_lock = threading.Lock()
_active_procs = set()

def _run_proc(cmd, cwd, env, timeout):
    """subprocess.Popen + communicate, tracked, same shape subprocess.run gave
    the caller before: (rc, combined stdout+stderr), rc == "TIMEOUT" on a
    timeout with whatever output had already been produced, exactly as
    subprocess.run's own TimeoutExpired handling captured it."""
    p = subprocess.Popen(cmd, cwd=cwd, env=env, stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE, text=True, errors="replace",
                         creationflags=NOCON, startupinfo=SI)
    with _proc_lock:
        _active_procs.add(p)
    try:
        try:
            out, err = p.communicate(timeout=timeout)
            return p.returncode, (out or "") + (err or "")
        except subprocess.TimeoutExpired:
            p.kill()
            out, err = p.communicate()
            return "TIMEOUT", (out or "") + (err or "")
    finally:
        with _proc_lock:
            _active_procs.discard(p)

_orig_sigint = signal.getsignal(signal.SIGINT)
def _sigint_handler(signum, frame):
    # kill only the children THIS process spawned, then fall through to the
    # normal Ctrl-C behaviour (KeyboardInterrupt) so the run still stops.
    with _proc_lock:
        procs = list(_active_procs)
    for p in procs:
        try: p.kill()
        except OSError: pass
    if callable(_orig_sigint):
        _orig_sigint(signum, frame)
    else:
        raise KeyboardInterrupt
signal.signal(signal.SIGINT, _sigint_handler)

# WORKER DIRECTORIES (run link100, lane SWEEPPAR1). Reuses battery.py's
# worker_dirs() design -- one private directory per worker, each holding its
# own copy of the exe, because crash.txt/exit.txt/startup_error.txt are
# EXE-RELATIVE (port/hal/instance_tag.h), not cwd-relative, so two rows
# sharing one folder would have the second faulting row overwrite the
# first's report -- but NOT battery.py's code, because battery.py copies
# into ROOT/build/battery-workers, and ROOT here is derived from the EXE
# under test (dirname of dirname of EXEDIR), which for this lane's own test
# binary is the READ-ONLY C:/tmp/sm64ds-playable: nothing may ever be
# written there. The scratch directory instead lives under THIS SCRIPT'S own
# tree (SELFROOT, three dirname()s up from bootab.py itself -- port/tools ->
# port -> the worktree root), so it always lands under whichever worktree is
# running the sweep, never under the tree that owns the exe being tested.
SELFROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
WORKER_BASE = os.path.join(SELFROOT, "tmp", "sweep-workers")

def make_worker_dirs(n, src_exe):
    """Return n directories, each with its own copy of src_exe. n<=1 returns
    [EXEDIR] and copies nothing, so the single-worker path touches no new
    files at all (same guarantee as battery.py's worker_dirs)."""
    if n <= 1:
        return [EXEDIR]
    dirs = []
    for i in range(n):
        d = os.path.join(WORKER_BASE, "w%d" % i)
        os.makedirs(d, exist_ok=True)
        dst = os.path.join(d, os.path.basename(src_exe))
        if (not os.path.exists(dst)
                or os.path.getmtime(dst) < os.path.getmtime(src_exe)
                or os.path.getsize(dst) != os.path.getsize(src_exe)):
            shutil.copy2(src_exe, dst)
        # a stale crash.txt/exit.txt beside a worker reads as THIS run's,
        # exactly the "stale artifact looks fresh" trap battery.py's
        # worker_dirs guards against the same way.
        for name in ART + ("startup_error.txt",):
            p = os.path.join(d, name)
            if os.path.exists(p):
                try: os.remove(p)
                except OSError: pass
        dirs.append(d)
    return dirs

def clear(wdir):
    for a in ART:
        p = os.path.join(wdir, a)
        if os.path.exists(p):
            try: os.remove(p)
            except OSError: pass
def run(kind, ident, label, ent=None, wdir=None):
    if wdir is None: wdir = EXEDIR
    clear(wdir)
    env = dict(base_env)
    env[kind] = str(ident)
    if wdir != EXEDIR:
        # the two names the port itself suffixes (startup_error.txt,
        # savestate.bin) and the window title, so a worker's own artifacts
        # never collide with another worker's -- battery.py's SM64DS_INSTANCE
        # convention, reused as-is.
        env["SM64DS_INSTANCE"] = os.path.basename(wdir)
    if ent is not None:
        # the PENDING entrance; Stage's own latch copies it to the current one
        # and level_boot passes that to LoadClsnAndObjects as the record index
        env["SM64DS_ENTRANCE"] = str(ent)
    if kind == "SM64DS_LEVEL":
        env["SM64DS_WINDOW_SELFTEST"] = FRAMES
        if IDLE: env["SM64DS_SELFTEST_IDLE"] = "1"
        if WARPIN or REENTRY:
            # the row's level is REACHED, not booted: castle grounds first, then
            # the game's own LoadLevelNoReturn. Level 1 cannot warp into itself,
            # so it uses level 6 as the other end of the change.
            other = 6 if int(ident) == 1 else 1
            if WARPIN:
                env[kind] = str(other)
                env["SM64DS_WARP_SEQ"] = "%d@600" % int(ident)
                env["SM64DS_WINDOW_SELFTEST"] = "1500"
            else:
                env["SM64DS_WARP_SEQ"] = "%d@400,%d@800" % (other, int(ident))
                env["SM64DS_WINDOW_SELFTEST"] = "1400"
            if PRESS and PRESS != "none": env["SM64DS_PROBE_INPUT"] = PRESS
        if EXIT:
            # LEAVING a course, which no other arm of this tool does. Each one
            # drives the cartridge's own exit and writes nothing but a position
            # or a pad word; walk_window.cpp's EXIT ARMS block is the whole of
            # them. The row is graded on the level change as well as the exit
            # code, because an exit that quietly does nothing leaves a live
            # level behind and exits 0.
            env["SM64DS_WINDOW_SELFTEST"] = "1200"
            env["SM64DS_EXIT_WATCH"] = "1"
            if EXIT == "void":
                # the death plane: drop below it at 200 and let
                # func_ov002_020c5d60 -> ST_DEAD_PIT -> HitDeathPlane run
                env["SM64DS_VOID_DROP"] = "200"
                # THE LEVEL-CLEAR SAVE MENU can open on this arm too: a level
                # booted directly at an entrance the game never sends a
                # player to can end its arrival animation in the one that
                # sets the star-get flag data_0209f20c, with no star
                # anywhere in the row, and the menu then freezes the world
                # before the death drop completes. The level-clear menu's
                # third row (Continue, Don't Save, which writes no save
                # file) is x 8..247, y 0x78..0x97 in
                # src/_ZN5Stage9LC_UpdateEv.cpp case 3; the pause menu's
                # four buttons are y 32-63, 72-103, 112-143 and 152-183, so
                # y=148 is inside the level-clear row and inside no pause
                # button, which is why every exit arm can carry the same
                # tap. The probe's frame numbers are POLL indices rather
                # than game frames, which is why the train is wide instead
                # of one tap.
                env["SM64DS_TOUCH_PROBE"] = ",".join(
                    "%d-%d:128:148" % (f, f + 1) for f in range(100, 3000, 40))
            elif EXIT == "star":
                # stand on the level's first PowerStar and press A through the
                # star-get prompts
                env["SM64DS_STAR_DROP"] = "#0,200"
                env["SM64DS_PROBE_INPUT"] = \
                    "260:A,300:A,340:A,380:A,420:A,460:A"
                # AFTER THE ARRIVAL, through the HOST INPUT LAYER. The A
                # pulses answer whatever the held-forward walk bumps into
                # in the castle (a star gate's message opened at f603 on
                # the level 6 row and never closed, which is what a
                # 1200-frame row used to end in); 4 is DPAD_LEFT, a
                # direction the selftest is NOT already holding, so the
                # movement it produces is the PLAYER'S and not the
                # harness's. The window opens at f500, long after the
                # change (f298 on the level 6 row), so no row's level
                # change verdict can move because of it.
                env["SM64DS_HOST_PAD"] = (
                    ",".join("1000@%d-%d" % (f, f + 3)
                             for f in range(500, 1160, 60))
                    + ",4@600-1150")
                # THE LEVEL-CLEAR SAVE MENU now opens after a star (the
                # star-get flag data_0209f20c drives Stage::LC_Update) and
                # freezes the world until it is answered, so this arm's own
                # "moved N after arrival" check fails every row unless
                # something answers it. Row three of the menu's own three
                # touch boxes (src/_ZN5Stage9LC_UpdateEv.cpp case 3: x
                # 8..247, y 0x78..0x97) is Continue, Don't Save, chosen over
                # row one (Save and Quit) because that row writes no file,
                # so a sweep leaves no saves behind in a worker's directory.
                # The probe's frame numbers are POLL indices, not game
                # frames, which is why this is a wide train rather than one
                # tap: a single poll index can land off-frame and miss the
                # menu entirely.
                env["SM64DS_TOUCH_PROBE"] = ",".join(
                    "%d-%d:128:148" % (f, f + 1) for f in range(300, 4000, 40))
            elif EXIT == "pause":
                # START, then the fourth pause button (exit course). The four
                # buttons are touch boxes x 8..247, y 0x20/0x48/0x70/0x98 each
                # 0x20 tall; the tap has to carry a press EDGE, so it is a
                # short range, not a hold.
                #
                # SEVEN STARTS, NOT ONE, and that is the cartridge's doing.
                # Stage::PS_Update case 2 (src/_ZN5Stage9PS_UpdateEv.cpp:404)
                # forks on Player::CanPause (0x020bd828 through
                # src/func_02029408.c), which returns 0 while the player is
                # AIRBORNE, taking damage or under no control. A START pressed
                # mid-stride lands in pause sub-state 0xb, which has no menu
                # buttons at all, and the run then reads as "the pause menu
                # ignores every tap" -- measured on 14, 15, 28, 33, 34 and
                # thirteen more. Retrying every 60 frames finds a grounded one.
                env["SM64DS_PROBE_INPUT"] = ",".join(
                    "%d:START" % f for f in range(200, 620, 60))
                env["SM64DS_TOUCH_PROBE"] = ",".join(
                    "%d-%d:128:168" % (f, f + 1) for f in range(230, 650, 60)
                ) + "," + ",".join(
                    "%d-%d:128:148" % (f, f + 1) for f in range(100, 3000, 40))
            else:
                sys.exit("unknown exit=%s (void, star, pause)" % EXIT)
    else: env["SM64DS_SCENE_FRAMES"] = FRAMES
    env["SM64DS_FAULTS_FATAL"] = "1"; env["SM64DS_NO_FOCUS"] = "1"; env["SM64DS_VOLUME"] = "0"
    # set on level and scene rows alike: the aspect is latched at boot, before
    # either path picks its presentation, so both read the same key
    if ASPECT: env["SM64DS_ASPECT"] = ASPECT
    # smooth=N, the same shape and the same reason as aspect= above: the
    # environment scrub drops an inherited SM64DS_SMOOTH_MODELS with the rest
    # of the SM64DS_* block, so a sweep that does not name it is byte-identical
    # to one from before this argument existed, and naming it is the only way
    # to reach the model smoother from here.
    if SMOOTH: env["SM64DS_SMOOTH_MODELS"] = SMOOTH
    # texfilter=N and aa=N (run hd2, lane PIC), the same shape and the same
    # reason as smooth= and aspect= above: the scrub drops an inherited
    # SM64DS_TEXTURE_FILTER / SM64DS_ANTI_ALIASING with the rest of the
    # SM64DS_* block, so a sweep that does not name either is byte-identical to
    # one from before these arguments existed, and naming one is the only way
    # to sweep the whole table with the sampler or the edge pass switched on.
    if TEXFILTER: env["SM64DS_TEXTURE_FILTER"] = TEXFILTER
    if AA: env["SM64DS_ANTI_ALIASING"] = AA
    # scale=N is the same again for "RenderScale" (run hd1, lane RES).
    # That lane had to put a settings.json beside the exe to sweep at a
    # scale, because the scrub drops the variable; this is the one line
    # that makes it an argument like the three above, and it is inert
    # unless it is named.
    if SCALE: env["SM64DS_RENDER_SCALE"] = SCALE
    # present=N and its four companions (run hd2, lane GPU1), the same shape
    # and the same reason as scale= above: the scrub drops an inherited
    # SM64DS_PRESENT_* / SM64DS_VSYNC with the rest of the SM64DS_* block, so a
    # sweep that names none of them is byte-identical to one from before these
    # arguments existed, and naming them is the only way to sweep the whole
    # table with the graphics-card present path switched on.
    if PRESENT: env["SM64DS_PRESENT_BACKEND"] = PRESENT
    if PRESENTFILTER: env["SM64DS_PRESENT_FILTER_D3D"] = PRESENTFILTER
    if VSYNC: env["SM64DS_VSYNC"] = VSYNC
    if PRESENTDEV: env["SM64DS_PRESENT_DEVICE"] = PRESENTDEV
    if PRESENTOFF: env["SM64DS_PRESENT_OFFSCREEN"] = PRESENTOFF
    # minimap=<size>, minimapdir=<folder>: the improved minimap's keys, the
    # same shape and the same reason as smooth= and aspect= above. They cannot
    # come from a settings.json here, for two reasons that are both about this
    # tool rather than about the option: at workers>1 only the exe is copied
    # into a worker directory, so a settings file beside the real exe is not
    # what a row reads; and the option's own getter is pinned OFF under
    # SM64DS_WINDOW_SELFTEST precisely so that a settings file cannot move a
    # recorded baseline, which leaves the environment as the only channel a
    # sweep has. Unset, both lines do nothing and a sweep is byte-identical to
    # one from before this argument existed.
    if MINIMAP:
        env["SM64DS_IMPROVED_MINIMAP"] = "1"
        env["SM64DS_MINIMAP_SCALE"] = MINIMAP
    if MINIMAPDIR: env["SM64DS_MINIMAP_DIR"] = MINIMAPDIR
    exe_path = os.path.join(wdir, os.path.basename(EXE))
    t0 = time.time()
    rc, out = _run_proc([exe_path], wdir, env, BUDGET)
    dt = time.time() - t0
    ok = rc == 0
    note = ""
    low = out.lower()
    for key in ("unhosted", "wrong bytes", "quarantine", "assert", "out of memory", "unhandled"):
        if key in low: note = key; break
    if rc == "TIMEOUT": note = (note + " " if note else "") + "timeout"
    elif isinstance(rc, int) and rc != 0: note = (note + " " if note else "") + "rc 0x%08x" % (rc & 0xFFFFFFFF)
    if EXIT and kind == "SM64DS_LEVEL":
        # an exit row that exits 0 but never left the level is a FAIL: the
        # whole point of the arm is the change, and a course that swallows it
        # leaves the player standing in a level he asked to leave.
        #
        # AND IT HAS TO BE THE RIGHT EXIT. data_0209f26c, the reason the change
        # carries, is 2 for a death, 1 for a course cleared and 0 for anything
        # else including exit-course. A selftest that walks a course for 1200
        # frames falls off plenty of them by itself, so a pause row that came
        # back with reason 2 measured a fall, not the pause menu -- eleven of
        # the first star sweep's twenty-six "passes" were that.
        want = {"void": 2, "star": 1, "pause": 0}.get(EXIT)
        chg = ""
        for line in out.splitlines():
            if line.startswith("[lvl] change:"): chg = line.strip(); break
        if not chg:
            ok = False
        elif want is not None and ("reason %d" % want) not in chg:
            ok = False
            chg += "  (WRONG EXIT: wanted reason %d)" % want
        note = (note + " | " if note else "") + (chg or "NO LEVEL CHANGE")
        # MOVES AFTER THE ARRIVAL. A change that completes and
        # leaves the player unable to move is the softlock Tango
        # reported, and until now this arm could not see it: the
        # [exitwatch] stream prints only on change, so a frozen
        # player prints nothing and a row that ends rc 0 reads as
        # a pass. [exitpos] is the position on a fixed cadence.
        # Take the samples from AFTER the pad window opens that
        # are in the level the change named, and require the
        # player to have moved more than a quarter of a walking
        # stride in some axis over them. A frozen player's
        # samples are byte-identical, so the threshold is not
        # near anything.
        if EXIT == "star" and ok:
            dest = None
            m = re.search(r"-> (-?\d+), entrance", chg)
            if m: dest = int(m.group(1))
            pts = []
            for line in out.splitlines():
                mm = re.match(r"\[exitpos\] f(\d+) level=(-?\d+) "
                              r"pos=\((-?\d+),(-?\d+),(-?\d+)\)",
                              line.strip())
                if not mm: continue
                if int(mm.group(1)) < 700: continue
                if dest is not None and int(mm.group(2)) != dest: continue
                pts.append(tuple(int(mm.group(i)) for i in (3, 4, 5)))
            if len(pts) < 4:
                ok = False
                note = (note + " | " if note else "") + \
                    "NO ARRIVAL POSITION SAMPLES (%d)" % len(pts)
            else:
                span = max(max(abs(p[i] - pts[0][i]) for p in pts)
                           for i in range(3))
                if span <= 16:
                    ok = False
                    note = (note + " | " if note else "") + \
                        ("PLAYER NEVER MOVED AFTER THE ARRIVAL "
                         "(span %d over %d samples)" % (span, len(pts)))
                else:
                    note = (note + " | " if note else "") + \
                        "moved %d after arrival" % span
    if ent is not None and kind == "SM64DS_LEVEL":
        # how far the held-forward walk actually got. An entrance whose mode
        # wedges the player in his arrival animation exits 0 with a position
        # that barely left the record's own spawn point, which no return code
        # can show.
        for line in out.splitlines():
            if line.startswith("selftest:"):
                note = (note + " | " if note else "") + line.strip(); break
    if not ok:
        d = os.path.join(OUT, "%s%d%s" % (label, ident,
                                          "" if ent is None else "_e%d" % ent))
        os.makedirs(d, exist_ok=True)
        for a in ART:
            p = os.path.join(wdir, a)
            if os.path.exists(p): shutil.copy2(p, d)
        with open(os.path.join(d, "stdout.txt"), "w", encoding="utf-8", errors="replace") as f: f.write(out)
        # one-line fault summary from the play log
        for line in out.splitlines():
            if line.startswith("FAULT") or "UNHOSTED" in line or "WRONG BYTES" in line:
                note += " | " + line.strip()[:160]; break
    return ok, rc, round(dt, 1), note

rows = []
def emit(label, i, ent, verdict, rc, dt, note):
    # ONE PLACE both the serial and the N-wide path print/record a finished
    # row, so the two cannot drift and the tsv/SUMMARY are identical in
    # shape whichever path ran them.
    if ENTRANCE:
        rows.append((label, i, "" if ent is None else ent, verdict, rc, dt, note))
        line = ("%-5s %-3d %-4s %-4s rc=%-12s %6.1fs %s"
                % (label, i, "" if ent is None else "e%d" % ent, verdict, rc, dt, note))
    else:
        rows.append((label, i, verdict, rc, dt, note))
        line = "%-5s %-3d %-4s rc=%-12s %6.1fs %s" % (label, i, verdict, rc, dt, note)
    print(line, flush=True)

plan = [("SM64DS_LEVEL", LEVELS, "level"), ("SM64DS_SCENE", SCENES, "scene")]
jobs = []  # (kind, id, label, entrance) in the exact order the serial loop below would visit them
for kind, ids, label in plan:
    for i in ids:
        if ENTRANCE and kind == "SM64DS_LEVEL":
            recs = list(range(ENTCOUNT.get(i, 0))) if ENTRANCE == "all" else [int(ENTRANCE)]
        else:
            recs = [None]
        for ent in recs:
            jobs.append((kind, i, label, ent))

if WORKERS <= 1:
    # UNCHANGED FROM BEFORE THIS LANE: one exe, one directory (EXEDIR, via
    # run()'s wdir=None default), rows run and printed one after another.
    for kind, i, label, ent in jobs:
        ok, rc, dt, note = run(kind, i, label, ent)
        emit(label, i, ent, "PASS" if ok else "FAIL", rc, dt, note)
else:
    # N ROWS AT A TIME (run link100, lane SWEEPPAR1), battery.py's own
    # design reused: a private directory per worker, checked OUT of a queue
    # for a row's whole lifetime and handed back when it finishes (never
    # indexed by i % workers -- see make_worker_dirs' docstring and
    # battery.py's level_row for why that matters), submitted in row order
    # and REPORTED in row order regardless of which one finishes first, so
    # the tsv and SUMMARY read exactly like a serial run, only faster.
    wdirs = make_worker_dirs(WORKERS, EXE)
    print("sweep: %d rows at a time, each in its own directory under %s"
          % (WORKERS, WORKER_BASE), flush=True)
    free_dirs = queue.Queue()
    for d in wdirs: free_dirs.put(d)
    def _job(kind, i, label, ent):
        d = free_dirs.get()
        try:
            return run(kind, i, label, ent, wdir=d)
        finally:
            free_dirs.put(d)
    with ThreadPoolExecutor(max_workers=WORKERS) as pool:
        futs = [pool.submit(_job, kind, i, label, ent) for (kind, i, label, ent) in jobs]
        for (kind, i, label, ent), f in zip(jobs, futs):
            ok, rc, dt, note = f.result()
            emit(label, i, ent, "PASS" if ok else "FAIL", rc, dt, note)

with open(os.path.join(OUT, "sweep.tsv"), "w") as f:
    f.write("kind\tid\tentrance\tverdict\trc\tseconds\tnote\n" if ENTRANCE
            else "kind\tid\tverdict\trc\tseconds\tnote\n")
    for r in rows: f.write("\t".join(str(x) for x in r) + "\n")
lv = [r for r in rows if r[0] == "level"]; sc = [r for r in rows if r[0] == "scene"]
V = 3 if ENTRANCE else 2   # the entrance arm puts the record between the id and the verdict
print("SUMMARY exe=%s%s%s%s%s levels %d/%d scenes %d/%d" % (EXE, " idle" if IDLE else "",
      (" warpin press=%s" % (PRESS or "none")) if WARPIN else ((" reentry press=%s" % (PRESS or "none")) if REENTRY else ""),
      " entrance=" + ENTRANCE if ENTRANCE else "",
      " aspect=" + ASPECT if ASPECT else " aspect=native", sum(r[V] == "PASS" for r in lv), len(lv), sum(r[V] == "PASS" for r in sc), len(sc)), flush=True)
