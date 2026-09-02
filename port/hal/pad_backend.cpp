/* The controller backend. Read pad_backend.h first; the banner there is the
   design. This file is the two backends, the layouts, the reconnect logic and
   the worker thread that keeps every slow call off the game thread.

   No static import of xinput*.dll, dinput8.dll or user32.dll: all three are
   LoadLibrary'd and every entry point is GetProcAddress'd. INITGUID makes
   dinput.h instantiate the interface and axis GUIDs in this translation unit,
   so neither dinput8.lib nor dxguid.lib is on the link line, and the joystick
   data format below is spelled by hand because the SDK's c_dfDIJoystick2 is a
   symbol in that lib rather than in the header.

   THREADS. Enumerating DirectInput devices costs hundreds of milliseconds to
   two seconds on a real machine (EnumDevices, then CreateDevice and a property
   read per device), and asking an EMPTY XInput slot is the one slow call in
   XInput. Neither may run on the game thread: a keyboard-only player would
   feel a hitch on every rescan and every launch would pay the enumeration
   before its first frame. So port_pad_init only reads its environment and
   starts a worker, and the worker owns all of that:

     * it loads both DLLs;
     * it scans the empty XInput slots (once a second, and at once on a
       device-change notification) and publishes the slot that answered;
     * it enumerates, acquires, polls and translates the DirectInput device
       and publishes the translated state under a lock;
     * it prints the "[pad] ..." line, once per change of answer.

   The game thread's port_pad_poll does exactly two cheap things: one
   XInputGetState on the slot already known to be connected, and one copy of
   the published DirectInput snapshot. It never enumerates and never sleeps.
   The DirectInput device is created, used and released on the worker only, so
   no interface pointer ever crosses a thread. */
#define DIRECTINPUT_VERSION 0x0800
#define INITGUID
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <dinput.h>
#ifndef DIDFT_OPTIONAL
#define DIDFT_OPTIONAL 0x80000000   /* older SDKs leave it to the caller */
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "hal/pad_backend.h"

/* XINPUT_GAMEPAD_* spelled here so no xinput.h is needed either. */
enum {
    XB_UP = 0x0001, XB_DOWN = 0x0002, XB_LEFT = 0x0004, XB_RIGHT = 0x0008,
    XB_START = 0x0010, XB_BACK = 0x0020, XB_LTHUMB = 0x0040, XB_RTHUMB = 0x0080,
    XB_LB = 0x0100, XB_RB = 0x0200,
    XB_A = 0x1000, XB_B = 0x2000, XB_X = 0x4000, XB_Y = 0x8000
};

/* SM64DS_PAD_BACKEND=xinput|dinput|none, read once in port_pad_init. Forcing
   dinput is how the fallback arm is exercised on a machine whose pad XInput
   already sees; none is a pad-less run on a machine with a pad. */
enum { MODE_AUTO, MODE_XINPUT, MODE_DINPUT, MODE_NONE };
static int g_mode = MODE_AUTO;

/* ------------------------------------------------------- shared state, lock */

static CRITICAL_SECTION g_lock;
static volatile LONG xi_slot = -1;      /* the XInput slot answering, or -1 */
static volatile LONG g_rescan_now = 0;  /* WM_DEVICECHANGE: scan on the next pass */
static int di_pub_live;                 /* under g_lock: a DirectInput pad answers */
static PortPadState di_pub_state;       /* under g_lock: its last translated state */
static char describe_buf[160] = "none"; /* under g_lock */
static char describe_out[160] = "none"; /* the caller-visible copy */

static void describe_set(const char *s)
{
    static char said[160];
    EnterCriticalSection(&g_lock);
    strncpy(describe_buf, s, sizeof describe_buf - 1);
    describe_buf[sizeof describe_buf - 1] = 0;
    int changed = strcmp(said, describe_buf) != 0;
    if (changed) strcpy(said, describe_buf);
    LeaveCriticalSection(&g_lock);
    /* one line per change of answer, never per pass */
    if (changed) {
        fprintf(stderr, "[pad] %s\n", s);
        fflush(stderr);
    }
}

/* ------------------------------------------------------------------ XInput */

static DWORD(WINAPI *XInputGetState_)(DWORD, PortPadState *);

static void xinput_load(void)
{
    const char *dlls[] = {"xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll"};
    for (int i = 0; i < 3 && !XInputGetState_; ++i)
        if (HMODULE x = LoadLibraryA(dlls[i]))
            XInputGetState_ =
                (decltype(XInputGetState_))GetProcAddress(x, "XInputGetState");
}

/* Every slot, slot 0 first. Worker only: an empty slot is slow to ask. */
static int xinput_scan(void)
{
    PortPadState t;
    for (DWORD s = 0; s < 4; ++s)
        if (XInputGetState_(s, &t) == ERROR_SUCCESS) return (int)s;
    return -1;
}

/* ------------------------------------------------------------- DirectInput */

typedef HRESULT(WINAPI *DirectInput8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID *,
                                              LPUNKNOWN);
static IDirectInput8A *di8;
static IDirectInputDevice8A *di_dev;    /* worker only */
static int di_acquired;
static char di_name[96];
static unsigned di_vid, di_pid;
static HWND di_hwnd;                    /* this process's top-level window, if found */

/* user32, for the cooperative level's window. Dynamic like everything else. */
static BOOL(WINAPI *EnumWindows_)(WNDENUMPROC, LPARAM);
static DWORD(WINAPI *GetWindowThreadProcessId_)(HWND, DWORD *);

/* One layout: which DirectInput button feeds each XInput bit, which buttons
   (or axes) are the triggers, and which axes are the right stick. Button
   numbers are the HID usage minus one, which is how DirectInput counts them.
   Axis indices: 0 X, 1 Y, 2 Z, 3 Rx, 4 Ry, 5 Rz. -1 means "not on this pad". */
struct PadLayout {
    unsigned short vid, pid;    /* 0,0 = the generic fallback */
    const char *name;
    signed char a, b, x, y, lb, rb, back, start, lthumb, rthumb;
    signed char lt_btn, rt_btn;
    signed char lt_axis, rt_axis;
    signed char rx_axis, ry_axis;
};

/* Face buttons are mapped by POSITION, not by the letter printed on them: the
   bottom button is XInput A, right is B, left is X, top is Y. That is what a
   player expects ("jump is the bottom button") and what every other layer of
   the port assumes about the mask. */
static const PadLayout PAD_LAYOUTS[] = {
    /* Nintendo Switch Pro Controller. HID order: Y B A X L R ZL ZR - + LS RS
       Home Capture. Nintendo's B is bottom, A right, Y left, X top. ZL/ZR are
       digital. Right stick on Z/Rz. D-pad is the hat. */
    {0x057e, 0x2009, "Switch Pro Controller",
     /* a b x y */ 1, 2, 0, 3, /* lb rb */ 4, 5, /* back start */ 8, 9,
     /* thumbs */ 10, 11, /* lt rt btn */ 6, 7, /* lt rt axis */ -1, -1,
     /* right stick */ 2, 5},
    /* Sony DualShock 4, both revisions. HID order: Square Cross Circle
       Triangle L1 R1 L2 R2 Share Options L3 R3 PS Touchpad. Cross is bottom.
       L2/R2 are also analog on Rx/Ry. Right stick on Z/Rz. Hat d-pad. */
    {0x054c, 0x05c4, "DualShock 4",
     1, 2, 0, 3, 4, 5, 8, 9, 10, 11, 6, 7, 3, 4, 2, 5},
    {0x054c, 0x09cc, "DualShock 4",
     1, 2, 0, 3, 4, 5, 8, 9, 10, 11, 6, 7, 3, 4, 2, 5},
    /* Sony DualSense: the DualShock 4's report layout. */
    {0x054c, 0x0ce6, "DualSense",
     1, 2, 0, 3, 4, 5, 8, 9, 10, 11, 6, 7, 3, 4, 2, 5},
    /* Everything else: buttons 0..3 as A B X Y, then LB RB LT RT Back Start
       LS RS in the order most HID pads report them, X/Y left stick, Z/Rz
       right stick, hat d-pad. An axis the pad does not have reads 0. */
    {0, 0, "generic",
     0, 1, 2, 3, 4, 5, 8, 9, 10, 11, 6, 7, -1, -1, 2, 5},
};
static const PadLayout *di_layout = &PAD_LAYOUTS[4];

/* The DIJOYSTATE2 data format, by hand (see the file banner). Every object
   is OPTIONAL so a pad with fewer axes or buttons still binds. */
#define DI_AX(guid, field) \
    {&guid, (DWORD)FIELD_OFFSET(DIJOYSTATE2, field), \
     DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, DIDOI_ASPECTPOSITION}
#define DI_POV(i) \
    {&GUID_POV, (DWORD)FIELD_OFFSET(DIJOYSTATE2, rgdwPOV[i]), \
     DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0}
#define DI_BTN(i) \
    {0, (DWORD)FIELD_OFFSET(DIJOYSTATE2, rgbButtons[i]), \
     DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL, 0}
static DIOBJECTDATAFORMAT DI_OBJS[] = {
    DI_AX(GUID_XAxis, lX),  DI_AX(GUID_YAxis, lY),  DI_AX(GUID_ZAxis, lZ),
    DI_AX(GUID_RxAxis, lRx), DI_AX(GUID_RyAxis, lRy), DI_AX(GUID_RzAxis, lRz),
    DI_AX(GUID_Slider, rglSlider[0]), DI_AX(GUID_Slider, rglSlider[1]),
    DI_POV(0), DI_POV(1), DI_POV(2), DI_POV(3),
    DI_BTN(0),  DI_BTN(1),  DI_BTN(2),  DI_BTN(3),  DI_BTN(4),  DI_BTN(5),
    DI_BTN(6),  DI_BTN(7),  DI_BTN(8),  DI_BTN(9),  DI_BTN(10), DI_BTN(11),
    DI_BTN(12), DI_BTN(13), DI_BTN(14), DI_BTN(15), DI_BTN(16), DI_BTN(17),
    DI_BTN(18), DI_BTN(19), DI_BTN(20), DI_BTN(21), DI_BTN(22), DI_BTN(23),
    DI_BTN(24), DI_BTN(25), DI_BTN(26), DI_BTN(27), DI_BTN(28), DI_BTN(29),
    DI_BTN(30), DI_BTN(31),
};
static const DIDATAFORMAT DI_FORMAT = {
    sizeof(DIDATAFORMAT), sizeof(DIOBJECTDATAFORMAT), DIDF_ABSAXIS,
    sizeof(DIJOYSTATE2), sizeof(DI_OBJS) / sizeof(DI_OBJS[0]), DI_OBJS};

/* The six axes' offsets in DIJOYSTATE2, whether the bound pad HAS each one,
   and its reported range (read back after asking for +-32767, so a pad that
   refuses the request still scales).

   PRESENCE MATTERS. An axis the pad lacks is left at 0 by GetDeviceState while
   its range stays at DirectInput's 0..65535 default, and scaling that gives
   -32767: a hard-over stick on an axis that does not exist. Every layout puts
   the right stick on Z/Rz and plenty of pads have no Rz (an Xbox pad seen
   through DirectInput, most cheap two-axis USB pads), so without this the
   camera would spin and pitch forever on them. Absent reads 0. */
static const DWORD DI_AXIS_OFS[6] = {
    FIELD_OFFSET(DIJOYSTATE2, lX),  FIELD_OFFSET(DIJOYSTATE2, lY),
    FIELD_OFFSET(DIJOYSTATE2, lZ),  FIELD_OFFSET(DIJOYSTATE2, lRx),
    FIELD_OFFSET(DIJOYSTATE2, lRy), FIELD_OFFSET(DIJOYSTATE2, lRz)};
static int di_axis_present[6];
static LONG di_axis_min[6], di_axis_max[6];

static BOOL CALLBACK di_axis_cb(const DIDEVICEOBJECTINSTANCEA *doi, void *)
{
    /* after SetDataFormat, dwOfs is the object's offset in OUR format */
    for (int a = 0; a < 6; ++a)
        if (doi->dwOfs == DI_AXIS_OFS[a]) di_axis_present[a] = 1;
    return DIENUM_CONTINUE;
}

/* An XInput-class device also shows up in DirectInput, with "IG_" in its
   device path. Step 1 already covers those, and reading one here as well
   would answer with a layout that is wrong for it (triggers on Z, and so
   on). */
static int di_is_xinput_device(IDirectInputDevice8A *dev)
{
    DIPROPGUIDANDPATH gp;
    memset(&gp, 0, sizeof gp);
    gp.diph.dwSize = sizeof gp;
    gp.diph.dwHeaderSize = sizeof gp.diph;
    gp.diph.dwObj = 0;
    gp.diph.dwHow = DIPH_DEVICE;
    if (FAILED(dev->GetProperty(DIPROP_GUIDANDPATH, &gp.diph))) return 0;
    return wcsstr(gp.wszPath, L"IG_") != 0 || wcsstr(gp.wszPath, L"ig_") != 0;
}

static void di_release_device(void)
{
    if (di_dev) {
        if (di_acquired) di_dev->Unacquire();
        di_dev->Release();
        di_dev = 0;
    }
    di_acquired = 0;
    EnterCriticalSection(&g_lock);
    di_pub_live = 0;
    LeaveCriticalSection(&g_lock);
}

/* THE FIRST NON-XINPUT GAME CONTROLLER WINS. DI8DEVCLASS_GAMECTRL covers
   wheels, flight sticks and pedal sets as well as pads, and this takes the
   first one attached with no attempt to tell them apart: a player with a
   racing wheel and a Pro Controller both plugged in may find the wheel
   answering. Unplugging it, or SM64DS_PAD_BACKEND=xinput, is the remedy
   until a device-type preference is worth its weight. */
static BOOL CALLBACK di_enum_cb(const DIDEVICEINSTANCEA *inst, void *)
{
    IDirectInputDevice8A *dev = 0;
    if (FAILED(di8->CreateDevice(inst->guidInstance, &dev, 0)) || !dev)
        return DIENUM_CONTINUE;
    if (di_is_xinput_device(dev)) {
        dev->Release();
        return DIENUM_CONTINUE;
    }
    if (FAILED(dev->SetDataFormat(&DI_FORMAT))) {
        dev->Release();
        return DIENUM_CONTINUE;
    }
    di_dev = dev;
    di_vid = inst->guidProduct.Data1 & 0xffff;
    di_pid = (inst->guidProduct.Data1 >> 16) & 0xffff;
    strncpy(di_name, inst->tszProductName, sizeof di_name - 1);
    di_name[sizeof di_name - 1] = 0;
    /* strip a trailing space or two some drivers leave on the name */
    for (size_t n = strlen(di_name); n && di_name[n - 1] == ' '; --n)
        di_name[n - 1] = 0;
    di_layout = &PAD_LAYOUTS[sizeof PAD_LAYOUTS / sizeof PAD_LAYOUTS[0] - 1];
    for (size_t i = 0; i + 1 < sizeof PAD_LAYOUTS / sizeof PAD_LAYOUTS[0]; ++i)
        if (PAD_LAYOUTS[i].vid == di_vid && PAD_LAYOUTS[i].pid == di_pid) {
            di_layout = &PAD_LAYOUTS[i];
            break;
        }
    /* which axes exist, then ask for +-32767 on each and read back what it
       really has. An axis whose range cannot be read is treated as absent. */
    memset(di_axis_present, 0, sizeof di_axis_present);
    dev->EnumObjects(di_axis_cb, 0, DIDFT_AXIS);
    for (int a = 0; a < 6; ++a) {
        di_axis_min[a] = 0;
        di_axis_max[a] = 65535;
        if (!di_axis_present[a]) continue;
        DIPROPRANGE r;
        memset(&r, 0, sizeof r);
        r.diph.dwSize = sizeof r;
        r.diph.dwHeaderSize = sizeof r.diph;
        r.diph.dwObj = DI_AXIS_OFS[a];
        r.diph.dwHow = DIPH_BYOFFSET;
        r.lMin = -32767;
        r.lMax = 32767;
        dev->SetProperty(DIPROP_RANGE, &r.diph);
        memset(&r, 0, sizeof r);
        r.diph.dwSize = sizeof r;
        r.diph.dwHeaderSize = sizeof r.diph;
        r.diph.dwObj = DI_AXIS_OFS[a];
        r.diph.dwHow = DIPH_BYOFFSET;
        if (SUCCEEDED(dev->GetProperty(DIPROP_RANGE, &r.diph)) && r.lMax > r.lMin) {
            di_axis_min[a] = r.lMin;
            di_axis_max[a] = r.lMax;
        } else {
            di_axis_present[a] = 0;
        }
    }
    return DIENUM_STOP;
}

static int dinput_load(void)
{
    if (di8) return 1;
    HMODULE d = LoadLibraryA("dinput8.dll");
    if (!d) return 0;
    DirectInput8Create_t create =
        (DirectInput8Create_t)GetProcAddress(d, "DirectInput8Create");
    if (!create) return 0;
    void *out = 0;
    if (FAILED(create(GetModuleHandleA(0), DIRECTINPUT_VERSION, IID_IDirectInput8A,
                      &out, 0)) ||
        !out)
        return 0;
    di8 = (IDirectInput8A *)out;
    if (HMODULE u = LoadLibraryA("user32.dll")) {
        EnumWindows_ = (decltype(EnumWindows_))GetProcAddress(u, "EnumWindows");
        GetWindowThreadProcessId_ = (decltype(GetWindowThreadProcessId_))
            GetProcAddress(u, "GetWindowThreadProcessId");
    }
    return 1;
}

/* Look for an attached, non-XInput game controller. Worker only. */
static int dinput_scan(void)
{
    if (!di8) return 0;
    di_release_device();
    di8->EnumDevices(DI8DEVCLASS_GAMECTRL, di_enum_cb, 0, DIEDFL_ATTACHEDONLY);
    return di_dev != 0;
}

static BOOL CALLBACK own_window_cb(HWND h, LPARAM)
{
    DWORD pid = 0;
    GetWindowThreadProcessId_(h, &pid);
    if (pid == GetCurrentProcessId()) {
        di_hwnd = h;
        return FALSE;
    }
    return TRUE;
}

static int di_acquire(void)
{
    if (!di_dev) return 0;
    if (di_acquired) return 1;
    /* background + non-exclusive: the caller's own focus gate decides who
       gets the input, the same as it does for XInput, so the device must
       keep answering when the window is not in front. The window is this
       process's own top-level one if it exists yet (the worker has no active
       window of its own, so it is looked up rather than asked for); NULL is
       accepted for a background level. A failure here is not fatal, Acquire
       below is the real test. */
    if (!di_hwnd && EnumWindows_ && GetWindowThreadProcessId_)
        EnumWindows_(own_window_cb, 0);
    di_dev->SetCooperativeLevel(di_hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(di_dev->Acquire())) return 0;
    di_acquired = 1;
    return 1;
}

static short di_axis(const DIJOYSTATE2 *js, int a)
{
    if (a < 0 || a > 5 || !di_axis_present[a]) return 0;
    LONG raw = *(const LONG *)((const char *)js + DI_AXIS_OFS[a]);
    LONG lo = di_axis_min[a], hi = di_axis_max[a];
    if (raw < lo) raw = lo;
    if (raw > hi) raw = hi;
    /* to +-32767 without overflowing 32 bits on a 0..65535 range */
    LONGLONG v = (LONGLONG)(raw - lo) * 65534 / (hi - lo) - 32767;
    return (short)v;
}

static unsigned char di_trigger(const DIJOYSTATE2 *js, int btn, int axis)
{
    unsigned char t = 0;
    if (btn >= 0 && btn < 128 && (js->rgbButtons[btn] & 0x80)) t = 255;
    if (axis >= 0 && axis < 6 && di_axis_present[axis]) {
        /* the axis rests at its minimum for a trigger; scale to 0..255 */
        int v = (int)di_axis(js, axis) + 32767;     /* 0..65534 */
        int s = v * 255 / 65534;
        if (s > t) t = (unsigned char)s;
    }
    return t;
}

static int di_btn(const DIJOYSTATE2 *js, int b)
{
    return b >= 0 && b < 128 && (js->rgbButtons[b] & 0x80);
}

static void di_translate(const DIJOYSTATE2 *js, PortPadState *out)
{
    const PadLayout *L = di_layout;
    unsigned m = 0;
    if (di_btn(js, L->a)) m |= XB_A;
    if (di_btn(js, L->b)) m |= XB_B;
    if (di_btn(js, L->x)) m |= XB_X;
    if (di_btn(js, L->y)) m |= XB_Y;
    if (di_btn(js, L->lb)) m |= XB_LB;
    if (di_btn(js, L->rb)) m |= XB_RB;
    if (di_btn(js, L->back)) m |= XB_BACK;
    if (di_btn(js, L->start)) m |= XB_START;
    if (di_btn(js, L->lthumb)) m |= XB_LTHUMB;
    if (di_btn(js, L->rthumb)) m |= XB_RTHUMB;
    /* the hat: hundredths of a degree clockwise from up, 0xFFFF centred */
    DWORD pov = js->rgdwPOV[0];
    if (LOWORD(pov) != 0xFFFF) {
        pov %= 36000;
        if (pov >= 31500 || pov <= 4500) m |= XB_UP;
        if (pov >= 4500 && pov <= 13500) m |= XB_RIGHT;
        if (pov >= 13500 && pov <= 22500) m |= XB_DOWN;
        if (pov >= 22500 && pov <= 31500) m |= XB_LEFT;
    }
    out->buttons = (unsigned short)m;
    out->lt = di_trigger(js, L->lt_btn, L->lt_axis);
    out->rt = di_trigger(js, L->rt_btn, L->rt_axis);
    /* DirectInput's Y grows downward; XInput's grows upward */
    out->lx = di_axis(js, 0);
    out->ly = (short)-di_axis(js, 1);
    out->rx = di_axis(js, L->rx_axis);
    out->ry = (short)-di_axis(js, L->ry_axis);
    /* a packet number that changes when the state does, which is all the
       callers ever ask of XInput's. Worker only, so plain statics. */
    static unsigned long pkt;
    static PortPadState last;
    if (memcmp(&last.buttons, &out->buttons, sizeof *out - sizeof out->packet))
        ++pkt;
    out->packet = pkt;
    last = *out;
}

/* One read of the bound device. 0 means "treat the device as lost": the
   caller releases it and the next scan finds it again if it is still there.
   One re-acquire is tried when input was lost; a second loss in the same
   pass returns rather than repeating the dance against GetDeviceState. */
static int di_read(PortPadState *out)
{
    if (!di_acquire()) return 0;
    HRESULT hr = di_dev->Poll();
    if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
        di_acquired = 0;
        if (!di_acquire()) return 0;
        hr = di_dev->Poll();
        if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) return 0;
    }
    /* DI_NOEFFECT is a success code: the device needs no polling */
    if (FAILED(hr)) return 0;
    DIJOYSTATE2 js;
    if (FAILED(di_dev->GetDeviceState(sizeof js, &js))) return 0;
    di_translate(&js, out);
    return 1;
}

/* ------------------------------------------------------------------ worker */

enum {
    XI_RESCAN_MS = 1000,    /* empty XInput slots, once a second */
    DI_RESCAN_MS = 2000,    /* DirectInput enumeration when nothing is bound */
    DI_POLL_MS = 4,         /* the bound device, ~250 Hz */
    IDLE_MS = 50            /* nothing bound: wake for the cadence only */
};

static void announce(void)
{
    char b[160];
    if (xi_slot >= 0) {
        snprintf(b, sizeof b, "XInput slot %d", (int)xi_slot);
    } else if (di_dev) {
        snprintf(b, sizeof b, "DirectInput: %s (%04x:%04x, %s layout)",
                 di_name[0] ? di_name : "unnamed controller", di_vid, di_pid,
                 di_layout->name);
    } else {
        strcpy(b, "none");
    }
    describe_set(b);
}

static DWORD WINAPI pad_worker(void *)
{
    if (g_mode != MODE_DINPUT) xinput_load();
    if (g_mode != MODE_XINPUT) dinput_load();
    DWORD last_xi = 0, last_di = 0;
    int first = 1;
    for (;;) {
        const DWORD now = GetTickCount();
        const int kick = InterlockedExchange(&g_rescan_now, 0) != 0;
        /* XInput: confirm the known slot, or look for one */
        if (XInputGetState_) {
            if (xi_slot >= 0) {
                if (first || kick || now - last_xi >= XI_RESCAN_MS) {
                    PortPadState t;
                    if (XInputGetState_((DWORD)xi_slot, &t) != ERROR_SUCCESS)
                        xi_slot = -1;
                    last_xi = now;
                }
            }
            if (xi_slot < 0 && (first || kick || now - last_xi >= XI_RESCAN_MS)) {
                xi_slot = xinput_scan();
                last_xi = now;
            }
        }
        /* DirectInput: bind, read, publish. NOT SCANNED WHILE XINPUT ANSWERS:
           the game thread takes the XInput slot first anyway, and an
           enumeration every two seconds is hundreds of milliseconds of CPU
           and HID traffic that every XInput user would pay forever. A swap
           to a non-XInput pad still arrives at once through the
           WM_DEVICECHANGE kick, and the cadence resumes when the slot goes. */
        if (di8) {
            if (!di_dev && (xi_slot < 0 || kick) &&
                (first || kick || now - last_di >= DI_RESCAN_MS)) {
                dinput_scan();
                last_di = now;
            }
            if (di_dev) {
                PortPadState s;
                if (di_read(&s)) {
                    EnterCriticalSection(&g_lock);
                    di_pub_live = 1;
                    di_pub_state = s;
                    LeaveCriticalSection(&g_lock);
                } else {
                    /* unplugged, or taken exclusively by someone else */
                    di_release_device();
                    last_di = now;
                }
            }
        }
        announce();
        first = 0;
        Sleep(di_dev ? DI_POLL_MS : IDLE_MS);
    }
}

/* ------------------------------------------------------------------ public */

static int inited;

int port_pad_init(void)
{
    if (inited) return g_mode != MODE_NONE;
    inited = 1;
    InitializeCriticalSection(&g_lock);
    if (const char *e = getenv("SM64DS_PAD_BACKEND")) {
        if (!_stricmp(e, "xinput")) g_mode = MODE_XINPUT;
        else if (!_stricmp(e, "dinput")) g_mode = MODE_DINPUT;
        else if (!_stricmp(e, "none")) g_mode = MODE_NONE;
        if (g_mode != MODE_AUTO) {
            fprintf(stderr, "[pad] forced: %s (SM64DS_PAD_BACKEND)\n",
                    g_mode == MODE_XINPUT ? "xinput only"
                    : g_mode == MODE_DINPUT ? "dinput only" : "none");
            fflush(stderr);
        } else {
            fprintf(stderr, "[pad] SM64DS_PAD_BACKEND=%s is not xinput, dinput "
                            "or none; ignored\n", e);
            fflush(stderr);
        }
    }
    if (g_mode == MODE_NONE) {
        describe_set("none (forced)");
        return 0;
    }
    /* everything slow happens on the worker; see the file banner */
    HANDLE t = CreateThread(0, 64 * 1024, pad_worker, 0, 0, 0);
    if (!t) {
        describe_set("none (worker thread failed)");
        return 0;
    }
    CloseHandle(t);
    return 1;
}

int port_pad_poll(PortPadState *out)
{
    if (!inited) port_pad_init();
    if (g_mode == MODE_NONE) return 0;
    /* the known XInput slot: one cheap call, never an empty slot */
    const LONG slot = xi_slot;
    if (slot >= 0 && XInputGetState_) {
        if (XInputGetState_((DWORD)slot, out) == ERROR_SUCCESS) return 1;
        xi_slot = -1;           /* unplugged: the worker looks again */
    }
    int live;
    EnterCriticalSection(&g_lock);
    live = di_pub_live;
    if (live) *out = di_pub_state;
    LeaveCriticalSection(&g_lock);
    return live;
}

void port_pad_device_changed(void)
{
    InterlockedExchange(&g_rescan_now, 1);
}

const char *port_pad_describe(void)
{
    if (!inited) return describe_out;
    EnterCriticalSection(&g_lock);
    strcpy(describe_out, describe_buf);
    LeaveCriticalSection(&g_lock);
    return describe_out;
}
