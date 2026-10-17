"""Find the Boos in Hide and Boo Seek (dScMgTeresa_c) through mgwatch's PEEK file,
then rub and tap over each in-play Boo.

usage: python tools/trace/mg/teresabot.py <scene addr hex> [seconds]

The scene address is r0 of any dScMgTeresa_c hit in the watcher log (or gMgScene, see
mgpeek.py). On a find it saves screenshots to build/melonds/shots/found*.png.
"""
import os, struct, subprocess, sys, time

HERE = os.path.dirname(os.path.abspath(__file__))
RUN = os.path.join(HERE, '..', '..', '..', 'build', 'melonds')
SCENE = int(sys.argv[1], 16)
SECONDS = float(sys.argv[2]) if len(sys.argv) > 2 else 20
BOOS = SCENE + 0x4660
PEEK = os.path.join(RUN, 'PEEK')


def peek_many(reqs):
    """reqs: [(addr, n)]; returns one bytes (or None) per request."""
    out = PEEK + '.out'
    if os.path.exists(out):
        os.remove(out)
    with open(PEEK, 'w') as f:
        for a, n in reqs:
            f.write('%x %x\n' % (a, n))
    for _ in range(200):
        if os.path.exists(out):
            time.sleep(0.05)
            res = []
            for line in open(out).read().splitlines():
                w = line.split()
                res.append(bytes.fromhex(w[1]) if len(w) == 2 else None)
            if len(res) == len(reqs):
                return res
        time.sleep(0.03)
    return [None] * len(reqs)


def boos():
    raws = peek_many([(BOOS + i * 0x24, 0x24) for i in range(16)])
    out = []
    for i, b in enumerate(raws):
        if b is None:
            continue
        x, y = struct.unpack_from('<ii', b, 0)
        out.append({'i': i, 'x': x >> 12, 'y': y >> 12, 'inPlay': b[0x17], 'state': b[0x18],
                    'visible': b[0x1a], 'found': b[0x1c], 'kind': b[0x1f]})
    return out


import ctypes, ctypes.wintypes as wt
u32 = ctypes.windll.user32
u32.SetProcessDPIAware()


def _window():
    found = []

    @ctypes.WINFUNCTYPE(ctypes.c_bool, wt.HWND, wt.LPARAM)
    def cb(hw, _):
        n = ctypes.create_unicode_buffer(256)
        u32.GetWindowTextW(hw, n, 256)
        if 'melonDS' in n.value and u32.IsWindowVisible(hw):
            found.append(hw)
        return True
    u32.EnumWindows(cb, 0)
    r = wt.RECT()
    u32.GetWindowRect(found[0], ctypes.byref(r))
    return r


R = _window()
TOP = R.top + 83
SCALE = ((R.bottom - 10) - TOP) / 384.0
LEFT = R.left + ((R.right - R.left) - 256 * SCALE) / 2.0


def move(x, y):
    x = max(1, min(254, x)); y = max(1, min(190, y))
    u32.SetCursorPos(int(LEFT + x * SCALE), int(TOP + 192 * SCALE + y * SCALE))


def down(): u32.mouse_event(2, 0, 0, 0, 0)
def up(): u32.mouse_event(4, 0, 0, 0, 0)


def scene_byte(off):
    r = peek_many([(SCENE + off, 1)])[0]
    return r[0] if r else None


def boo(i):
    b = peek_many([(BOOS + i * 0x24, 0x24)])[0]
    if b is None:
        return None
    x, y = struct.unpack_from('<ii', b, 0)
    return {'x': x / 4096.0, 'y': y / 4096.0, 'inPlay': b[0x17], 'state': b[0x18], 'found': b[0x1c]}


t0 = time.time()
holding = False
prev = None
shots_taken = False
while time.time() - t0 < SECONDS:
    if scene_byte(0x4c20) == 0:
        if holding:
            up(); holding = False
        prev = None
        time.sleep(0.05)
        continue
    b = boo(0)
    now = time.time()
    if not b or not b['inPlay'] or b['found']:
        if holding:
            up(); holding = False
        if b and b['found'] and not shots_taken:
            print(round(now - t0, 1), 'FOUND', flush=True)
            shots_taken = True
            for k in range(14):
                subprocess.Popen(['powershell', '-NoProfile', '-File', os.path.join(HERE, 'shot.ps1'),
                                  '-Out', os.path.join(RUN, 'shots', 'found%d.png' % k), '-Delay', str(0.3 * k)])
        time.sleep(0.05)
        continue
    vx = vy = 0
    if prev:
        dt = now - prev[0]
        vx, vy = (b['x'] - prev[1]) / dt, (b['y'] - prev[2]) / dt
    prev = (now, b['x'], b['y'])
    move(b['x'] + vx * 0.08, b['y'] + vy * 0.08)
    if not holding:
        down(); holding = True
    time.sleep(0.02)
if holding:
    up()
