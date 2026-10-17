"""Time taps in the Toad face-slot game (dScMgSlot3_c) so a column lines up on the target.

A row stops on the first slot boundary it reaches after its touch (Slot3_State1), so a
tap must land while the target piece is within one face width before the anchor slot.
Pieces are found by colour: Wario's yellow cap, his pink nose, his pink tongue.

usage: python tools/trace/mg/slotbot.py probe          # print colour runs per band
       python tools/trace/mg/slotbot.py play           # stop hat, then eyes, then mouth
"""
import ctypes, ctypes.wintypes as wt, sys, time

u32, g32 = ctypes.windll.user32, ctypes.windll.gdi32
u32.SetProcessDPIAware()

def window():
    h = u32.FindWindowW(None, None)
    found = []
    @ctypes.WINFUNCTYPE(ctypes.c_bool, wt.HWND, wt.LPARAM)
    def cb(hw, _):
        n = ctypes.create_unicode_buffer(256); u32.GetWindowTextW(hw, n, 256)
        if 'melonDS' in n.value and u32.IsWindowVisible(hw):
            found.append(hw)
        return True
    u32.EnumWindows(cb, 0)
    r = wt.RECT(); u32.GetWindowRect(found[0], ctypes.byref(r))
    return found[0], r

HW, R = window()
TOP = R.top + 83
SCALE = ((R.bottom - 10) - TOP) / 384.0
LEFT = R.left + ((R.right - R.left) - 256 * SCALE) / 2.0
def ds2win(x, y): return int(LEFT + x * SCALE), int(TOP + 192 * SCALE + y * SCALE)
X0, _ = ds2win(0, 0); X1, _ = ds2win(256, 0)
W = X1 - X0
SPACING = 80 * SCALE * 256 / 256 * (256 / 256)   # one face = 80 units; 1 unit = 1 DS px here
SPACING = 80 * SCALE
PERIOD = 3 * SPACING                               # level 0: strip length 3

class BMI(ctypes.Structure):
    _fields_ = [('biSize', wt.DWORD), ('biWidth', ctypes.c_long), ('biHeight', ctypes.c_long),
                ('biPlanes', wt.WORD), ('biBitCount', wt.WORD), ('biCompression', wt.DWORD),
                ('biSizeImage', wt.DWORD), ('a', ctypes.c_long), ('b', ctypes.c_long),
                ('c', wt.DWORD), ('d', wt.DWORD)]

def grab(y0, y1):
    """BGRA rows y0..y1 (window-screen coords) across the bottom screen."""
    h = y1 - y0
    sdc = u32.GetDC(0); mdc = g32.CreateCompatibleDC(sdc)
    bmp = g32.CreateCompatibleBitmap(sdc, W, h); g32.SelectObject(mdc, bmp)
    g32.BitBlt(mdc, 0, 0, W, h, sdc, X0, y0, 0x00CC0020)
    bi = BMI(ctypes.sizeof(BMI), W, -h, 1, 32, 0, 0, 0, 0, 0, 0)
    buf = ctypes.create_string_buffer(W * h * 4)
    g32.GetDIBits(mdc, bmp, 0, h, buf, ctypes.byref(bi), 0)
    g32.DeleteObject(bmp); g32.DeleteDC(mdc); u32.ReleaseDC(0, sdc)
    return buf.raw, h

def yellow(b, g, r): return r > 200 and g > 170 and b < 90
def pink(b, g, r):   return r > 190 and 70 < g < 160 and 120 < b < 210

BANDS = {  # DS y range sampled for each row, and its colour test
    0: ((70, 84), yellow),
    1: ((99, 109), pink),
    2: ((112.4, 116), pink),
}

MINC = {0: 2, 1: 2, 2: 1}

def runs(row):
    (ya, yb), test = BANDS[row]
    _, y0 = ds2win(0, ya); _, y1 = ds2win(0, yb)
    raw, h = grab(y0, y1)
    col = [0] * W
    for y in range(0, h, 2):
        base = y * W * 4
        for x in range(0, W, 2):
            i = base + x * 4
            if test(raw[i], raw[i + 1], raw[i + 2]):
                col[x] += 1
    out, x = [], 0
    while x < W:
        if col[x] >= MINC[row]:
            s = x
            while x < W and (col[x] >= MINC[row] or (x + 4 < W and col[x + 4] >= MINC[row])):
                x += 2
            if x - s > 12:
                out.append((s + x) / 2.0)
        x += 2
    return out

def phase(row):
    """Piece position folded into one period, or None."""
    r = runs(row)
    # a piece cut by the screen edge reads off-centre; prefer runs well inside
    inner = [c for c in r if SPACING * 0.4 < c < W - SPACING * 0.4]
    if not inner:
        return None
    return inner[0] % PERIOD

def tap(dsy):
    x, y = ds2win(128, dsy)
    u32.SetCursorPos(x, y)
    u32.mouse_event(2, 0, 0, 0, 0); time.sleep(0.08); u32.mouse_event(4, 0, 0, 0, 0)

ROW_Y = {0: 76, 1: 100, 2: 124}

def stop_on(row, anchor, speed_units):
    """Tap `row` so its Wario piece stops at `anchor` (folded x)."""
    v_px = speed_units / 4096.0 * SCALE * 60     # px per second, unsigned
    # direction from two samples
    p1 = None
    while p1 is None:
        p1 = phase(row); t1 = time.perf_counter()
    time.sleep(0.03)
    p2 = None
    while p2 is None:
        p2 = phase(row); t2 = time.perf_counter()
    d = ((p2 - p1 + PERIOD / 2) % PERIOD) - PERIOD / 2
    sign = 1 if d > 0 else -1
    print('row %d: dir %+d (moved %.1f px in %.3fs; expect %.1f)' % (row, sign, d, t2 - t1, v_px * (t2 - t1)))
    LAT = 0.06
    deadline = time.time() + 8
    while time.time() < deadline:
        p = phase(row)
        if p is None:
            continue
        rem = ((anchor - p) * sign) % PERIOD        # distance still to travel to the anchor
        want = SPACING * 0.5 + v_px * LAT
        if abs(rem - want) < v_px * 0.03 + 6:
            tap(ROW_Y[row])
            print('row %d: tapped at rem %.1f (want %.1f)' % (row, rem, want))
            return True
    print('row %d: no tap window found' % row)
    return False

def main():
    if sys.argv[1] == 'probe':
        for row in (0, 1, 2):
            print(row, [round(c) for c in runs(row)])
        return
    # stop the hat row anywhere, then line the others up under its Wario cap
    tap(ROW_Y[0]); time.sleep(0.6)
    a = phase(0)
    print('anchor (cap) at %s' % a)
    if a is None:
        return
    stop_on(1, a, 0x4ccc)       # second stop: row 1 speed tab[1*3+1]
    time.sleep(0.6)
    stop_on(2, a, 0x6999)       # third stop: row 2 speed tab[2*3+2]

if __name__ == "__main__":
    main()
