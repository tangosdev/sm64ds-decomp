"""emuwin: melonDS window plumbing that never needs keyboard focus (Windows, stdlib).

Used by emusession.py / ask.py. Everything here talks to ONE window handle:
- capture:   PrintWindow(PW_CLIENTONLY | PW_RENDERFULLCONTENT) into a memory DC.
             Works while the window is behind other windows (not minimised).
- input:     WM_KEYDOWN/WM_KEYUP and mouse messages POSTED to the handle. No
             SetForegroundWindow, no SendInput, so a dialog that steals focus
             cannot swallow (or receive) the keys.
- menus:     savestates go through melonDS's own File menu, driven by UI
             Automation (uia_menu.ps1), which also works without focus.
- launch:    the INSTALLED melonDS.exe (one exe path = one firewall prompt,
             ever), with ask.py's melonDS.toml swapped in next to it (portable
             mode) and the user's toml backed up in the work dir and restored,
             hash-checked, when the session stops (`ask.py restore-config`).
"""
import ctypes
import ctypes.wintypes as wt
import os
import shutil
import struct
import subprocess
import time
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))

user32 = ctypes.windll.user32
gdi32 = ctypes.windll.gdi32
try:
    ctypes.windll.shcore.SetProcessDpiAwareness(2)
except Exception:  # older Windows
    try:
        user32.SetProcessDPIAware()
    except Exception:
        pass

user32.PostMessageW.argtypes = [wt.HWND, wt.UINT, wt.WPARAM, wt.LPARAM]
user32.PrintWindow.argtypes = [wt.HWND, wt.HDC, wt.UINT]
gdi32.GetDIBits.argtypes = [wt.HDC, wt.HBITMAP, wt.UINT, wt.UINT, ctypes.c_void_p, ctypes.c_void_p, wt.UINT]


# --------------------------------------------------------------------- frames
class Frame:
    """An RGB image held as raw bytes (row-major, 3 bytes per pixel)."""

    def __init__(self, w, h, rgb):
        self.w, self.h, self.rgb = w, h, bytes(rgb)

    def crop(self, x, y, w, h):
        x, y = max(0, x), max(0, y)
        w, h = min(w, self.w - x), min(h, self.h - y)
        out = bytearray()
        for r in range(y, y + h):
            o = (r * self.w + x) * 3
            out += self.rgb[o:o + w * 3]
        return Frame(w, h, out)

    def row(self, y):
        o = y * self.w * 3
        return self.rgb[o:o + self.w * 3]

    def diff_fraction(self, other, tol=24):
        """Fraction of pixels whose max channel delta exceeds tol (sizes must match)."""
        if (self.w, self.h) != (other.w, other.h):
            return 1.0
        a, b = self.rgb, other.rgb
        n = 0
        step = 3 * 2  # sample every other pixel; plenty for a change detector
        for i in range(0, len(a), step):
            if (abs(a[i] - b[i]) > tol or abs(a[i + 1] - b[i + 1]) > tol
                    or abs(a[i + 2] - b[i + 2]) > tol):
                n += 1
        return n / max(1, len(a) // step)

    def save_png(self, path):
        raw = bytearray()
        stride = self.w * 3
        for r in range(self.h):
            raw.append(0)
            raw += self.rgb[r * stride:(r + 1) * stride]

        def chunk(tag, data):
            c = struct.pack('>I', len(data)) + tag + data
            return c + struct.pack('>I', zlib.crc32(tag + data) & 0xffffffff)
        png = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', self.w, self.h, 8, 2, 0, 0, 0))
        png += chunk(b'IDAT', zlib.compress(bytes(raw), 6)) + chunk(b'IEND', b'')
        os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
        with open(path, 'wb') as f:
            f.write(png)
        return path

    @staticmethod
    def hstack(frames, gap=8, gap_rgb=(255, 255, 255)):
        h = max(f.h for f in frames)
        w = sum(f.w for f in frames) + gap * (len(frames) - 1)
        g = bytes(gap_rgb) * gap
        out = bytearray()
        for r in range(h):
            parts = []
            for f in frames:
                parts.append(f.row(r) if r < f.h else bytes(f.w * 3))
            out += g.join(parts)
        return Frame(w, h, out)


# --------------------------------------------------------------------- window
def find_window(pid=None):
    """Largest visible top-level Qt window titled 'melonDS...' (optionally of pid)."""
    found = []

    @ctypes.WINFUNCTYPE(ctypes.c_bool, wt.HWND, wt.LPARAM)
    def cb(h, _l):
        if not user32.IsWindowVisible(h):
            return True
        n = user32.GetWindowTextLengthW(h)
        b = ctypes.create_unicode_buffer(n + 1)
        user32.GetWindowTextW(h, b, n + 1)
        if 'melonDS' not in b.value:
            return True
        c = ctypes.create_unicode_buffer(64)
        user32.GetClassNameW(h, c, 64)
        if not c.value.startswith('Qt'):
            return True
        if pid is not None:
            p = wt.DWORD()
            user32.GetWindowThreadProcessId(h, ctypes.byref(p))
            if p.value != pid:
                return True
        found.append(h)
        return True
    user32.EnumWindows(cb, 0)
    if not found:
        return None

    def area(h):
        r = wt.RECT()
        user32.GetWindowRect(h, ctypes.byref(r))
        return (r.right - r.left) * (r.bottom - r.top)
    return max(found, key=area)


def client_size(hwnd):
    r = wt.RECT()
    user32.GetClientRect(hwnd, ctypes.byref(r))
    return r.right, r.bottom


def set_client_size(hwnd, w, h):
    """Resize without activating (SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE)."""
    wr, cr = wt.RECT(), wt.RECT()
    user32.GetWindowRect(hwnd, ctypes.byref(wr))
    user32.GetClientRect(hwnd, ctypes.byref(cr))
    dw = (wr.right - wr.left) - cr.right
    dh = (wr.bottom - wr.top) - cr.bottom
    user32.SetWindowPos(hwnd, 0, 0, 0, w + dw, h + dh, 0x0002 | 0x0004 | 0x0010)


class _BMIH(ctypes.Structure):
    _fields_ = [('biSize', wt.DWORD), ('biWidth', wt.LONG), ('biHeight', wt.LONG),
                ('biPlanes', wt.WORD), ('biBitCount', wt.WORD), ('biCompression', wt.DWORD),
                ('biSizeImage', wt.DWORD), ('biXPelsPerMeter', wt.LONG), ('biYPelsPerMeter', wt.LONG),
                ('biClrUsed', wt.DWORD), ('biClrImportant', wt.DWORD)]


def capture_client(hwnd):
    """The window's client area as a Frame, rendered by DWM (no focus, no z-order)."""
    w, h = client_size(hwnd)
    if w <= 0 or h <= 0:
        raise RuntimeError('window has no client area (minimised?)')
    hdc = user32.GetDC(hwnd)
    mem = gdi32.CreateCompatibleDC(hdc)
    bmp = gdi32.CreateCompatibleBitmap(hdc, w, h)
    old = gdi32.SelectObject(mem, bmp)
    try:
        if not user32.PrintWindow(hwnd, mem, 1 | 2):
            raise RuntimeError('PrintWindow failed')
        bmi = _BMIH()
        bmi.biSize = ctypes.sizeof(_BMIH)
        bmi.biWidth, bmi.biHeight = w, -h
        bmi.biPlanes, bmi.biBitCount = 1, 32
        buf = ctypes.create_string_buffer(w * h * 4)
        gdi32.SelectObject(mem, old)
        if gdi32.GetDIBits(mem, bmp, 0, h, buf, ctypes.byref(bmi), 0) != h:
            raise RuntimeError('GetDIBits failed')
        bgra = buf.raw
        rgb = bytearray(w * h * 3)
        rgb[0::3] = bgra[2::4]
        rgb[1::3] = bgra[1::4]
        rgb[2::3] = bgra[0::4]
        return Frame(w, h, rgb)
    finally:
        gdi32.DeleteObject(bmp)
        gdi32.DeleteDC(mem)
        user32.ReleaseDC(hwnd, hdc)


def menubar_height(frame):
    """Rows at the top whose right-hand quarter is the flat colour of pixel
    (w-1, 0): the Qt menu bar's background past its last item (any theme)."""
    ref = frame.row(0)[-3:]
    x0 = frame.w * 3 // 4
    for y in range(min(120, frame.h)):
        r = frame.row(y)
        for x in range(x0, frame.w, 4):
            px = r[3 * x:3 * x + 3]
            if max(abs(px[i] - ref[i]) for i in range(3)) > 6:
                return y
    return 0


def screen_rects(frame):
    """(top, bottom) DS screen rects (x, y, w, h) inside a client-area capture.

    Assumes the layout ask.py writes into melonDS.toml: natural/vertical layout,
    even sizing, no gap, no rotation. The screen panel is everything below the
    Qt menu bar, and the two 256x192 screens are scaled to fit and centred in it.
    """
    y0 = menubar_height(frame)
    pw, ph = frame.w, frame.h - y0
    s = min(pw / 256.0, ph / 384.0)
    sw, sh = int(round(256 * s)), int(round(192 * s))
    x = (pw - sw) // 2
    y = y0 + (ph - 2 * sh) // 2
    return (x, y, sw, sh), (x, y + sh, sw, sh)


def fit_window(hwnd, scale=2):
    """Size the client area so both screens are drawn at exactly `scale`x."""
    mb = menubar_height(capture_client(hwnd))
    set_client_size(hwnd, 256 * scale, 384 * scale + mb)
    time.sleep(0.3)
    return mb


def capture_screens(hwnd, which='top'):
    f = capture_client(hwnd)
    top, bot = screen_rects(f)
    if which == 'top':
        return f.crop(*top)
    if which == 'bottom':
        return f.crop(*bot)
    return f.crop(top[0], top[1], top[2], top[3] + bot[3])


# ---------------------------------------------------------------------- input
# The key map ask.py writes into its private melonDS.toml (Qt key codes), and the
# Windows virtual keys that produce them.
QT_KEYS = {'A': 16777236, 'B': 16777237, 'X': 16777235, 'Y': 16777234,   # arrows
           'Up': 87, 'Down': 83, 'Left': 65, 'Right': 68,               # W S A D
           'Start': 39, 'Select': 16777233, 'L': 81, 'R': 69}           # ' End Q E
VK = {'A': 0x27, 'B': 0x28, 'X': 0x26, 'Y': 0x25, 'UP': 0x57, 'DOWN': 0x53, 'LEFT': 0x41,
      'RIGHT': 0x44, 'START': 0xDE, 'SELECT': 0x23, 'L': 0x51, 'R': 0x45}
_EXTENDED = {0x25, 0x26, 0x27, 0x28, 0x23}


def _lparam(vk, up):
    sc = user32.MapVirtualKeyW(vk, 0)
    lp = 1 | (sc << 16) | ((1 if vk in _EXTENDED else 0) << 24)
    if up:
        lp |= (1 << 30) | (1 << 31)
    return lp


def press(hwnd, buttons, ms=100):
    """Hold DS buttons (names from VK) for ms, via posted messages only.
    The emulator must be RUNNING (not halted at a breakpoint) for the game to see it."""
    vks = [VK[b.upper()] for b in buttons]
    for v in vks:
        user32.PostMessageW(hwnd, 0x100, v, _lparam(v, False))
    time.sleep(ms / 1000.0)
    for v in reversed(vks):
        user32.PostMessageW(hwnd, 0x101, v, _lparam(v, True))
    time.sleep(0.03)


def touch(hwnd, dx, dy, ms=120):
    """Tap the bottom screen at DS coordinates (0..255, 0..191)."""
    f = capture_client(hwnd)
    _top, (x, y, w, h) = screen_rects(f)
    px = int(x + (dx + 0.5) * w / 256.0)
    py = int(y + (dy + 0.5) * h / 192.0)
    lp = (py << 16) | (px & 0xffff)
    user32.PostMessageW(hwnd, 0x0200, 0, lp)
    user32.PostMessageW(hwnd, 0x0201, 1, lp)
    time.sleep(ms / 1000.0)
    user32.PostMessageW(hwnd, 0x0202, 0, lp)


def close_menus(pid=None):
    """Escape any open popup menu windows (titled exactly 'melonDS')."""
    pops = []

    @ctypes.WINFUNCTYPE(ctypes.c_bool, wt.HWND, wt.LPARAM)
    def cb(h, _l):
        n = user32.GetWindowTextLengthW(h)
        b = ctypes.create_unicode_buffer(n + 1)
        user32.GetWindowTextW(h, b, n + 1)
        if b.value == 'melonDS' and user32.IsWindowVisible(h):
            pops.append(h)
        return True
    user32.EnumWindows(cb, 0)
    for h in pops:
        user32.PostMessageW(h, 0x100, 0x1B, 0x00010001)
        user32.PostMessageW(h, 0x101, 0x1B, 0xC0010001)
        time.sleep(0.1)
    return len(pops)


def menu(pid, path, timeout=30):
    """Click a melonDS menu path, e.g. ['File', 'Load state', '1'], via UI Automation."""
    ps = os.path.join(HERE, 'uia_menu.ps1')
    r = subprocess.run(['powershell', '-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', ps,
                        '-ProcessId', str(pid), '-Path', '|'.join(path)],
                       capture_output=True, text=True, timeout=timeout)
    time.sleep(0.3)
    close_menus(pid)
    out = (r.stdout + r.stderr).strip()
    if r.returncode != 0:
        raise RuntimeError('menu %s failed: %s' % ('>'.join(path), out))
    return out


def load_state_slot(pid, slot):
    return menu(pid, ['File', 'Load state', str(slot)])


def save_state_slot(pid, slot):
    return menu(pid, ['File', 'Save state', str(slot)])


# --------------------------------------------------------------------- launch
TOML = '''# written by tools/trace/emuwin.py (ask.py session); the user's own config is in the ask.py work dir
LastROMFolder = ""
PauseLostFocus = false
LimitFPS = true
TargetFPS = 60.0
AudioSync = false

[Emu]
DirectBoot = true
ExternalBIOSEnable = false
ConsoleType = 0

[JIT]
Enable = false

[Screen]
UseGL = false

[3D]
Renderer = 0

[Savestate]
RelocSRAM = false

[Instance0]
SavestatePath = ""
SaveFilePath = ""
EnableCheats = false

[Instance0.Gdb]
Enabled = true

[Instance0.Gdb.ARM9]
Port = {port9}
BreakOnStartup = false

[Instance0.Gdb.ARM7]
Port = {port7}
BreakOnStartup = false

[Instance0.Audio]
Volume = 0

[Instance0.Window0]
Enabled = true
ScreenLayout = 0
ScreenSizing = 0
ScreenRotation = 0
ScreenGap = 0
ScreenSwap = false
IntegerScaling = false
ScreenAspectTop = 0
ScreenAspectBot = 0
ShowOSD = false

[Instance0.Keyboard]
{keys}
'''


def find_melonds():
    exe = shutil.which('melonDS') or shutil.which('melonDS.exe')
    if exe:
        exe = os.path.realpath(exe)
        if os.path.isfile(exe):
            return exe
    base = os.path.join(os.environ.get('LOCALAPPDATA', ''), 'Microsoft', 'WinGet', 'Packages')
    if os.path.isdir(base):
        for d in os.listdir(base):
            if d.lower().startswith('melonds.melonds'):
                p = os.path.join(base, d, 'melonDS.exe')
                if os.path.isfile(p):
                    return p
    raise RuntimeError('melonDS.exe not found (install melonDS 1.1 or pass --melonds)')


BACKUP = 'melonDS.toml.user-backup'


def _sha(path):
    import hashlib
    with open(path, 'rb') as f:
        return hashlib.sha256(f.read()).hexdigest()


def swap_config(workdir, exe, port=3333):
    """Put ask.py's config next to the INSTALLED melonDS.exe, keeping the user's.

    The exe is never copied: Windows Firewall asks once per executable path
    (the GDB stub listens on a socket), so a copied exe re-prompts every time.
    The user's melonDS.toml goes to workdir/melonDS.toml.user-backup (+ .sha256)
    and comes back in restore_config(). If a backup is already there (a session
    that died before restoring), it is kept: the live toml is ours, not theirs.
    """
    cfg = os.path.join(os.path.dirname(exe), 'melonDS.toml')
    bak = os.path.join(workdir, BACKUP)
    if not os.path.exists(bak):
        if os.path.exists(cfg):
            shutil.copy2(cfg, bak)
            with open(bak + '.sha256', 'w') as f:
                f.write(_sha(cfg) + ' ' + cfg + '\n')
        else:
            with open(bak + '.sha256', 'w') as f:
                f.write('absent ' + cfg + '\n')
    keys = '\n'.join('%s = %d' % (k, v) for k, v in QT_KEYS.items())
    with open(cfg, 'w', newline='\n') as f:
        f.write(TOML.replace('{port9}', str(port)).replace('{port7}', str(port + 1)).replace('{keys}', keys))
    return cfg


def restore_config(workdir):
    """Undo swap_config. Returns (cfg path, sha256 now, sha256 recorded) or None."""
    bak = os.path.join(workdir, BACKUP)
    if not os.path.exists(bak + '.sha256'):
        return None
    want, cfg = open(bak + '.sha256').read().strip().split(' ', 1)
    if want == 'absent':
        if os.path.exists(cfg):
            os.remove(cfg)
        os.remove(bak + '.sha256')
        return cfg, 'absent', 'absent'
    shutil.copy2(bak, cfg)
    got = _sha(cfg)
    if got == want:
        os.remove(bak)
        os.remove(bak + '.sha256')
    return cfg, got, want


def prepare_workdir(workdir, rom, state=None, slot=1, melonds=None, port=3333, sav=None):
    """Work dir = ROM copy + savestate slot; config swapped in next to the installed exe."""
    os.makedirs(workdir, exist_ok=True)
    exe = melonds or find_melonds()
    swap_config(workdir, exe, port)
    game = os.path.join(workdir, 'game.nds')
    if not os.path.exists(game) or os.path.getsize(game) != os.path.getsize(rom):
        shutil.copy2(rom, game)
    if sav:
        shutil.copy2(sav, os.path.join(workdir, 'game.sav'))
    if state:
        shutil.copy2(state, os.path.join(workdir, 'game.ml%d' % slot))
    return exe, game


def launch(workdir, exe=None, wait_window=30):
    exe = exe or find_melonds()
    game = os.path.join(workdir, 'game.nds')
    p = subprocess.Popen([exe, game], cwd=workdir, creationflags=0x00000008 | 0x00000200)
    t0 = time.time()
    while time.time() - t0 < wait_window:
        h = find_window(p.pid)
        if h:
            return p, h
        if p.poll() is not None:
            raise RuntimeError('melonDS exited with %s' % p.returncode)
        time.sleep(0.25)
    raise RuntimeError('melonDS window did not appear')


def kill(pid):
    subprocess.run(['taskkill', '/PID', str(pid), '/F'], capture_output=True)
