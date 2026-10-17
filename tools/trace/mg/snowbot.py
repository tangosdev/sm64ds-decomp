"""Push the snowball up the course in Snowball (dScMgSnowball_c) with repeated upward
swipes, reading the ball through mgwatch's PEEK file.

usage: python tools/trace/mg/snowbot.py [seconds] [steer]

Any upward drag adds to the ball's velocity, so repeated swipes push it; [steer]
scales a sideways correction against the ball's x speed. It stops when the run
reaches phase 2 (the goal) or ends.
"""
import ctypes, ctypes.wintypes as wt, os, struct, sys, time
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from mgpeek import peek_many, GMGSCENE  # noqa: E402

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


def swipe(x0, y0, x1, y1, steps=6, dt=0.012):
    move(x0, y0)
    u32.mouse_event(2, 0, 0, 0, 0)
    for k in range(1, steps + 1):
        time.sleep(dt)
        move(x0 + (x1 - x0) * k / steps, y0 + (y1 - y0) * k / steps)
    time.sleep(dt)
    u32.mouse_event(4, 0, 0, 0, 0)


SCENE = struct.unpack('<I', peek_many([(GMGSCENE, 4)])[0])[0]


def ball():
    r = peek_many([(SCENE + 0xab38, 0x30), (SCENE + 0xaba0, 4), (SCENE + 0xb9dc, 4), (SCENE + 0xb9f4, 4)])
    if None in r:
        return None
    x, y = struct.unpack_from('<ii', r[0], 0)
    vx, vy = struct.unpack_from('<ii', r[0], 0x28)
    return {'x': x / 4096, 'y': y / 4096, 'vx': vx / 4096, 'vy': vy / 4096,
            'size': struct.unpack('<i', r[1])[0] / 4096, 'clock': struct.unpack('<i', r[2])[0],
            'phase': struct.unpack('<i', r[3])[0]}


SECONDS = float(sys.argv[1]) if len(sys.argv) > 1 else 45
STEER = float(sys.argv[2]) if len(sys.argv) > 2 else 0.0
t0 = time.time()
last = 0
while time.time() - t0 < SECONDS:
    b = ball()
    if b and time.time() - last > 1:
        last = time.time()
        print(round(time.time() - t0, 1), {k: round(v, 1) for k, v in b.items()}, flush=True)
    if b and b['phase'] >= 2:
        print('phase', b['phase'], 'reached', flush=True)
        break
    dx = 0
    if b and STEER:
        dx = -b['vx'] * STEER
    swipe(128, 170, 128 + dx, 70)
