/* THE CONTROLLER BACKEND: one normalized pad state, whatever is plugged in.
 *
 * WHAT WAS WRONG. walk_window.cpp loaded XInput dynamically and polled slot 0,
 * and only slot 0. Players reported that a Nintendo Switch Pro Controller and
 * "some other" controllers did nothing at all, and both halves of that report
 * are the same bug: XInput sees only Xbox-class pads (and a Pro Controller or
 * a DualShock is not one unless Steam or a driver is translating for it), and
 * an Xbox pad that XInput has seated in slot 1 -- which happens when a
 * wireless receiver or a second pad has been and gone -- was never asked.
 *
 * WHAT THIS IS. A single poll that answers in XINPUT'S OWN SHAPE (the button
 * mask, the two sticks at +-32767 with Y positive up, the two triggers at
 * 0..255), tried in order:
 *
 *   1. XInput, slots 0..3, first connected wins;
 *   2. DirectInput8, the first attached game controller that is NOT an XInput
 *      device (those carry "IG_" in their device path and are already covered
 *      by step 1), with its buttons and axes mapped onto the XInput mask by a
 *      known layout for the Switch Pro Controller (057e:2009), the DualShock 4
 *      (054c:05c4, 054c:09cc) and the DualSense (054c:0ce6), and a plain
 *      positional fallback for everything else.
 *
 * The shape is walk_window.cpp's XPad, kept byte-for-byte so that file's
 * focus gate, scripted pad, menu, rebind capture and DS keypad mapping do not
 * change at all. An XInput user sees exactly what they saw before, with slot 0
 * still asked first.
 *
 * NO STATIC IMPORT. Both DLLs are LoadLibrary'd and every entry point is
 * GetProcAddress'd, the same rule walk_window.cpp applies to user32 and
 * xinput and hal/sub_screen.cpp applies to user32: the shipped exe carries
 * no import of either, so a machine without dinput8.dll (there is none, but
 * the rule is the rule) still starts. The DirectInput interfaces and GUIDs
 * come out of the SDK's dinput.h with INITGUID, so no dinput8.lib or
 * dxguid.lib is linked, and the joystick data format is spelled by hand
 * because the SDK's c_dfDIJoystick2 lives in that lib.
 *
 * DISCONNECT AND RECONNECT. A pad that goes away reads as not live from that
 * frame (the caller zeroes its copy on that edge already, see pad_focus_gate).
 * XInput slots that answered "not connected" are re-asked on a slow cadence
 * rather than every frame, because XInputGetState on an empty slot is the one
 * slow call in the whole input path. A DirectInput device whose input is lost
 * is re-acquired, and if that fails the device list is enumerated again on the
 * same slow cadence, so plugging a pad in mid-play picks it up without a
 * restart.
 *
 * SELFTEST. port_pad_poll is never called by a headless selftest -- the
 * caller gates it, as it always did -- so a drifting stick cannot perturb the
 * battery. port_pad_init runs regardless and prints its one line, which is
 * the line a bug report needs:
 *
 *   [pad] XInput slot 0
 *   [pad] DirectInput: Pro Controller (057e:2009)
 *   [pad] none
 */
#ifndef PORT_PAD_BACKEND_H
#define PORT_PAD_BACKEND_H

/* Identical in layout to XINPUT_GAMEPAD behind a packet number, i.e. to the
   XINPUT_STATE XInputGetState fills, and to walk_window.cpp's XPad. */
struct PortPadState {
    unsigned long packet;
    unsigned short buttons;     /* the XINPUT_GAMEPAD_* mask */
    unsigned char lt, rt;       /* 0..255 */
    short lx, ly, rx, ry;       /* +-32767, Y positive up */
};

/* Load the backends and look once for a pad. Prints the one "[pad] ..." line.
   Returns nonzero if either backend loaded at all (not whether a pad was
   found); a zero return means no poll can ever succeed. Safe to call more
   than once; only the first call does anything. */
int port_pad_init(void);

/* Read the current pad. Returns 1 and fills *out when a pad is connected and
   answering; returns 0 and leaves *out untouched otherwise, which is exactly
   what XInputGetState(0, &pad) == 0 used to mean. */
int port_pad_poll(PortPadState *out);

/* The backend and device the last successful discovery named, for a log or
   an overlay. "none" until something answers. */
const char *port_pad_describe(void);

#endif /* PORT_PAD_BACKEND_H */
