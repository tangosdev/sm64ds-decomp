"""On-screen detector for the opening/title STAR SNAP (run rel037 lane OPENSTAR).

WHY THIS DOES NOT READ A FRAMEBUFFER BMP. A capture written by the renderer
answers "what did the rasteriser produce", which is a different question from
"what is on the screen": this tree has already had a bug (the opening's black
bottom half) where the framebuffer BMP was whole while the window was visibly
half black, because the defect lived in the blit. So this samples the REAL
WINDOW CLIENT AREA off the desktop with GDI BitBlt from the screen DC, which
is CopyFromScreen semantics, and analyses those pixels.

WHAT IT MEASURES. The title's bottom DS screen is the lower half of the
stacked client area. The star is the only large yellow object on it. Its
share of that half is ~2.4% when the star is the right size and ~69% when the
scale bug has blown it up by 16x, so the two states are not close and a single
threshold separates them with a wide margin.

WHY IT SAMPLES OVER TIME RATHER THAN AT A NAMED FRAME. The defect is periodic
-- the star's rotation cosine goes negative for about 43 frames out of every
86 -- so a run of a few seconds passes through the broken phase several times.
Sampling every ~150 ms for the whole run and taking the MAXIMUM coverage seen
catches it without having to synchronise to a frame number, and makes the
detector independent of where in its spin the star starts.

USAGE
  python port/tools/star_snap_check.py <path-to-walk_window.exe> [seconds]

Exit code 0 = PASS (star never blew up), 1 = FAIL (it did), 2 = harness error.
"""
import ctypes, ctypes.wintypes as wt, os, subprocess, sys, time

u32, g32, k32 = ctypes.windll.user32, ctypes.windll.gdi32, ctypes.windll.kernel32
SRCCOPY = 0x00CC0020
STAR_MAX = 0.15          # fixed ~0.024, broken ~0.69; anywhere between works

def find_window(pid, timeout=25.0):
    """The first visible 'sm64ds_walk' window owned by pid."""
    found = []
    CB = ctypes.WINFUNCTYPE(ctypes.c_bool, wt.HWND, wt.LPARAM)
    def cb(hwnd, _):
        p = wt.DWORD()
        u32.GetWindowThreadProcessId(hwnd, ctypes.byref(p))
        if p.value != pid or not u32.IsWindowVisible(hwnd):
            return True
        buf = ctypes.create_string_buffer(64)
        u32.GetClassNameA(hwnd, buf, 64)
        if buf.value == b"sm64ds_walk":
            found.append(hwnd); return False
        return True
    end = time.time() + timeout
    while time.time() < end and not found:
        u32.EnumWindows(CB(cb), 0)
        if found: return found[0]
        time.sleep(0.05)
    return None

def client_rect_on_screen(hwnd):
    r = wt.RECT(); u32.GetClientRect(hwnd, ctypes.byref(r))
    pt = wt.POINT(0, 0); u32.ClientToScreen(hwnd, ctypes.byref(pt))
    return pt.x, pt.y, r.right, r.bottom

class BITMAPINFOHEADER(ctypes.Structure):
    _fields_ = [("biSize", wt.DWORD), ("biWidth", ctypes.c_long),
                ("biHeight", ctypes.c_long), ("biPlanes", wt.WORD),
                ("biBitCount", wt.WORD), ("biCompression", wt.DWORD),
                ("biSizeImage", wt.DWORD), ("biXPelsPerMeter", ctypes.c_long),
                ("biYPelsPerMeter", ctypes.c_long), ("biClrUsed", wt.DWORD),
                ("biClrImportant", wt.DWORD)]
class BITMAPINFO(ctypes.Structure):
    _fields_ = [("bmiHeader", BITMAPINFOHEADER), ("bmiColors", wt.DWORD * 3)]

def grab(x, y, w, h):
    """BitBlt the given screen rectangle and return it as top-down BGRA bytes."""
    sdc = u32.GetDC(0)
    mdc = g32.CreateCompatibleDC(sdc)
    bmp = g32.CreateCompatibleBitmap(sdc, w, h)
    g32.SelectObject(mdc, bmp)
    g32.BitBlt(mdc, 0, 0, w, h, sdc, x, y, SRCCOPY)
    bi = BITMAPINFO()
    bi.bmiHeader.biSize = ctypes.sizeof(BITMAPINFOHEADER)
    bi.bmiHeader.biWidth = w
    bi.bmiHeader.biHeight = -h           # negative => top-down rows
    bi.bmiHeader.biPlanes = 1
    bi.bmiHeader.biBitCount = 32
    buf = ctypes.create_string_buffer(w * h * 4)
    g32.GetDIBits(mdc, bmp, 0, h, buf, ctypes.byref(bi), 0)
    g32.DeleteObject(bmp); g32.DeleteDC(mdc); u32.ReleaseDC(0, sdc)
    return buf.raw

def star_share(px, w, h):
    """Fraction of the BOTTOM half that is star yellow."""
    tot = yel = 0
    for row in range(h // 2, h):
        base = row * w * 4
        for x in range(0, w, 2):
            o = base + x * 4
            b, g, r = px[o], px[o + 1], px[o + 2]
            tot += 1
            if r > 150 and g > 110 and b < 110 and r >= g:
                yel += 1
    return (yel / tot) if tot else 0.0

def main():
    if len(sys.argv) < 2:
        print(__doc__); return 2
    exe = os.path.abspath(sys.argv[1])
    secs = float(sys.argv[2]) if len(sys.argv) > 2 else 6.0
    env = {k: v for k, v in os.environ.items() if not k.startswith("SM64DS_")}
    env.update({
        "SM64DS_SCENE": "1",
        "SM64DS_SCENE_WINDOW": "1",
        "SM64DS_SCENE_FRAMES": str(int(secs * 60) + 240),
        "SM64DS_WINDOW_POS": "0,0",
        "SM64DS_TEST_LOCK": "1",
        "SM64DS_TEST_LOCK_PATH": r"C:\tmp\sm64ds-test-slot\windowed_test.lock",
        "SM64DS_TEST_LOCK_TIMEOUT": "5400",
    })
    proc = subprocess.Popen([exe], cwd=os.path.dirname(exe), env=env,
                            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        hwnd = find_window(proc.pid)
        if not hwnd:
            print("star_snap_check: HARNESS ERROR - no sm64ds_walk window appeared")
            return 2
        u32.SetForegroundWindow(hwnd)
        time.sleep(0.4)
        x, y, w, h = client_rect_on_screen(hwnd)
        if w < 64 or h < 64:
            print("star_snap_check: HARNESS ERROR - client area %dx%d" % (w, h))
            return 2
        print("star_snap_check: window %dx%d at (%d,%d), sampling %.1fs"
              % (w, h, x, y, secs))
        shares, end = [], time.time() + secs
        while time.time() < end and proc.poll() is None:
            shares.append(star_share(grab(x, y, w, h), w, h))
            time.sleep(0.15)
    finally:
        if proc.poll() is None:
            proc.kill(); proc.wait()
    if not shares:
        print("star_snap_check: HARNESS ERROR - no samples taken")
        return 2
    hi, lo = max(shares), min(shares)
    print("star_snap_check: %d samples, star share min %.4f max %.4f "
          "(threshold %.2f)" % (len(shares), lo, hi, STAR_MAX))
    if hi > STAR_MAX:
        print("star_snap_check: FAIL - the star blew up on screen "
              "(%.1f%% of the bottom screen)" % (hi * 100))
        return 1
    print("star_snap_check: PASS - the star stayed its own size for the "
          "whole run")
    return 0

if __name__ == "__main__":
    sys.exit(main())
