/* The controller backend. Read pad_backend.h first; the banner there is the
   design. This file is the two backends, the layouts and the reconnect logic.

   No static import of xinput*.dll or dinput8.dll: both are LoadLibrary'd and
   every entry point is GetProcAddress'd. INITGUID makes dinput.h instantiate
   the interface and axis GUIDs in this translation unit, so neither
   dinput8.lib nor dxguid.lib is on the link line, and the joystick data
   format below is spelled by hand because the SDK's c_dfDIJoystick2 is a
   symbol in that lib rather than in the header. */
#define DIRECTINPUT_VERSION 0x0800
#define INITGUID
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <dinput.h>
#ifndef DIDFT_OPTIONAL
#define DIDFT_OPTIONAL 0x80000000   /* older SDKs leave it to the caller */
#endif
#include <stdio.h>
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

/* ------------------------------------------------------------------ XInput */

static DWORD(WINAPI *XInputGetState_)(DWORD, PortPadState *);
static int xi_slot = -1;        /* the slot answering now, or -1 */
static int xi_rescan = 0;       /* frames until empty slots are asked again */
enum { XI_RESCAN_FRAMES = 60 }; /* once a second at the game's own rate */

static int xinput_load(void)
{
    const char *dlls[] = {"xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll"};
    for (int i = 0; i < 3 && !XInputGetState_; ++i)
        if (HMODULE x = LoadLibraryA(dlls[i]))
            XInputGetState_ =
                (decltype(XInputGetState_))GetProcAddress(x, "XInputGetState");
    return XInputGetState_ != 0;
}

/* Every slot, slot 0 first. Returns the slot that answered or -1. */
static int xinput_scan(PortPadState *out)
{
    if (!XInputGetState_) return -1;
    for (DWORD s = 0; s < 4; ++s)
        if (XInputGetState_(s, out) == ERROR_SUCCESS) return (int)s;
    return -1;
}

static int xinput_poll(PortPadState *out)
{
    if (!XInputGetState_) return 0;
    if (xi_slot >= 0) {
        if (XInputGetState_((DWORD)xi_slot, out) == ERROR_SUCCESS) return 1;
        xi_slot = -1;                   /* unplugged: fall through to a rescan */
        xi_rescan = 0;
    }
    if (--xi_rescan > 0) return 0;
    xi_rescan = XI_RESCAN_FRAMES;
    xi_slot = xinput_scan(out);
    return xi_slot >= 0;
}

/* ------------------------------------------------------------- DirectInput */

typedef HRESULT(WINAPI *DirectInput8Create_t)(HINSTANCE, DWORD, REFIID, LPVOID *,
                                              LPUNKNOWN);
static IDirectInput8A *di8;
static IDirectInputDevice8A *di_dev;
static int di_acquired;
static int di_rescan;
enum { DI_RESCAN_FRAMES = 120 };
static char di_name[96];
static unsigned di_vid, di_pid;
static HWND(WINAPI *GetActiveWindow_)(void);

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
       right stick, hat d-pad. */
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

/* The six axes' offsets in DIJOYSTATE2 and their reported ranges, read back
   after asking for +-32767 so a pad that refuses the request still scales. */
static const DWORD DI_AXIS_OFS[6] = {
    FIELD_OFFSET(DIJOYSTATE2, lX),  FIELD_OFFSET(DIJOYSTATE2, lY),
    FIELD_OFFSET(DIJOYSTATE2, lZ),  FIELD_OFFSET(DIJOYSTATE2, lRx),
    FIELD_OFFSET(DIJOYSTATE2, lRy), FIELD_OFFSET(DIJOYSTATE2, lRz)};
static LONG di_axis_min[6], di_axis_max[6];

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
}

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
    /* ask for +-32767 on every axis, then read back what each really has */
    for (int a = 0; a < 6; ++a) {
        DIPROPRANGE r;
        memset(&r, 0, sizeof r);
        r.diph.dwSize = sizeof r;
        r.diph.dwHeaderSize = sizeof r.diph;
        r.diph.dwObj = DI_AXIS_OFS[a];
        r.diph.dwHow = DIPH_BYOFFSET;
        r.lMin = -32767;
        r.lMax = 32767;
        dev->SetProperty(DIPROP_RANGE, &r.diph);
        di_axis_min[a] = 0;
        di_axis_max[a] = 65535;         /* DirectInput's default if unasked */
        memset(&r, 0, sizeof r);
        r.diph.dwSize = sizeof r;
        r.diph.dwHeaderSize = sizeof r.diph;
        r.diph.dwObj = DI_AXIS_OFS[a];
        r.diph.dwHow = DIPH_BYOFFSET;
        if (SUCCEEDED(dev->GetProperty(DIPROP_RANGE, &r.diph)) && r.lMax > r.lMin) {
            di_axis_min[a] = r.lMin;
            di_axis_max[a] = r.lMax;
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
    if (HMODULE u = LoadLibraryA("user32.dll"))
        GetActiveWindow_ =
            (decltype(GetActiveWindow_))GetProcAddress(u, "GetActiveWindow");
    return 1;
}

/* Look for an attached, non-XInput game controller. Returns 1 if di_dev is
   set afterwards. */
static int dinput_scan(void)
{
    if (!di8) return 0;
    di_release_device();
    di8->EnumDevices(DI8DEVCLASS_GAMECTRL, di_enum_cb, 0, DIEDFL_ATTACHEDONLY);
    return di_dev != 0;
}

static int di_acquire(void)
{
    if (!di_dev) return 0;
    if (di_acquired) return 1;
    /* background + non-exclusive: the caller's own focus gate decides who
       gets the input, the same as it does for XInput, so the device must
       keep answering when the window is not in front. The hwnd is whichever
       window is active; NULL is accepted for a background level. A failure
       here is not fatal, Acquire below is the real test. */
    HWND hwnd = GetActiveWindow_ ? GetActiveWindow_() : 0;
    di_dev->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(di_dev->Acquire())) return 0;
    di_acquired = 1;
    return 1;
}

static short di_axis(const DIJOYSTATE2 *js, int a)
{
    if (a < 0 || a > 5) return 0;
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
    if (axis >= 0) {
        /* the axis rests at its minimum for a trigger; scale 0..255 */
        int v = (int)di_axis(js, axis) + 32767;     /* 0..65534 */
        int s = v / 257;
        if (s > 255) s = 255;
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
       callers ever ask of XInput's */
    static unsigned long pkt;
    static PortPadState last;
    if (memcmp(&last.buttons, &out->buttons, sizeof *out - sizeof out->packet))
        ++pkt;
    out->packet = pkt;
    last = *out;
}

static int dinput_poll(PortPadState *out)
{
    if (!di8) return 0;
    if (!di_dev) {
        if (--di_rescan > 0) return 0;
        di_rescan = DI_RESCAN_FRAMES;
        if (!dinput_scan()) return 0;
    }
    if (!di_acquire()) return 0;
    HRESULT hr = di_dev->Poll();
    if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
        di_acquired = 0;
        if (!di_acquire()) goto lost;
        hr = di_dev->Poll();
    }
    /* DI_NOEFFECT is a success code: the device needs no polling */
    if (FAILED(hr) && hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED &&
        hr != DI_NOEFFECT)
        goto lost;
    {
        DIJOYSTATE2 js;
        hr = di_dev->GetDeviceState(sizeof js, &js);
        if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
            di_acquired = 0;
            if (!di_acquire()) goto lost;
            hr = di_dev->GetDeviceState(sizeof js, &js);
        }
        if (FAILED(hr)) goto lost;
        di_translate(&js, out);
        return 1;
    }
lost:
    /* unplugged (or taken exclusively by someone else): drop it and look
       again on the slow cadence */
    di_release_device();
    di_rescan = DI_RESCAN_FRAMES;
    return 0;
}

/* ------------------------------------------------------------------ public */

static char describe_buf[160] = "none";
static char describe_said[160];
static int inited;

static void describe_set(const char *s)
{
    strncpy(describe_buf, s, sizeof describe_buf - 1);
    describe_buf[sizeof describe_buf - 1] = 0;
    /* one line per change of answer, never per frame */
    if (strcmp(describe_said, describe_buf) != 0) {
        strcpy(describe_said, describe_buf);
        fprintf(stderr, "[pad] %s\n", describe_buf);
        fflush(stderr);
    }
}

static void describe_xinput(int slot)
{
    char b[64];
    snprintf(b, sizeof b, "XInput slot %d", slot);
    describe_set(b);
}

static void describe_dinput(void)
{
    char b[160];
    snprintf(b, sizeof b, "DirectInput: %s (%04x:%04x, %s layout)",
             di_name[0] ? di_name : "unnamed controller", di_vid, di_pid,
             di_layout->name);
    describe_set(b);
}

int port_pad_init(void)
{
    if (inited) return XInputGetState_ != 0 || di8 != 0;
    inited = 1;
    int xi = xinput_load();
    int di = dinput_load();
    PortPadState tmp;
    xi_slot = xinput_scan(&tmp);
    xi_rescan = XI_RESCAN_FRAMES;
    if (xi_slot >= 0) {
        describe_xinput(xi_slot);
    } else if (di && dinput_scan()) {
        di_rescan = DI_RESCAN_FRAMES;
        describe_dinput();
    } else {
        describe_set("none");
    }
    return xi || di;
}

int port_pad_poll(PortPadState *out)
{
    if (!inited) port_pad_init();
    PortPadState s;
    if (xinput_poll(&s)) {
        *out = s;
        describe_xinput(xi_slot);
        return 1;
    }
    if (dinput_poll(&s)) {
        *out = s;
        describe_dinput();
        return 1;
    }
    describe_set("none");
    return 0;
}

const char *port_pad_describe(void)
{
    return describe_buf;
}
