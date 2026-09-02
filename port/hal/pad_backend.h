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
 * A WORKER THREAD OWNS EVERY SLOW CALL. Enumerating DirectInput devices is
 * hundreds of milliseconds to two seconds; asking an empty XInput slot is
 * milliseconds. Both run on a thread port_pad_init starts, which publishes
 * the connected XInput slot and the translated DirectInput state; the game
 * thread's port_pad_poll makes one XInputGetState on a slot already known to
 * be connected and copies one snapshot, and never blocks. Init itself only
 * reads the environment and starts the thread, so it is NON-BLOCKING: a pad
 * reads not-live until the worker's first pass has finished (about half a
 * second, the DLL loads and the first enumeration), and the "[pad]" line can
 * trail the boot by that one pass. The DirectInput scan is skipped while an
 * XInput slot answers. See the banner in the .cpp.
 *
 * SM64DS_PAD_BACKEND=xinput|dinput|none forces one arm (or no pad at all),
 * read once in port_pad_init and announced with a "[pad] forced: ..." line.
 * This is how the DirectInput arm is exercised on a machine whose pad XInput
 * already sees.
 *
 * A LEARNED LAYOUT BEATS THE TABLE. A DirectInput pad the table does not know
 * gets the positional guess, and a player reported the guess wrong on theirs
 * (only the d-pad worked, the face buttons came out rotated). The debug
 * menu's "pad layout" row walks the player through one press per control and
 * hands the result to port_pad_set_layout, keyed by the pad's vid:pid; it is
 * applied on the worker's next pass with no restart, and the "[pad]" line
 * then says "(learned layout)". settings.json's PadLayouts array
 * (hal/host_settings.h) carries it between runs; port_pad_init seeds the
 * override table from there. port_pad_raw is the learn flow's window onto
 * the raw button indices and axes, before any layout is applied.
 *
 * SELFTEST. port_pad_poll is never called by a headless selftest -- the
 * caller gates it, as it always did -- so a drifting stick cannot perturb the
 * battery. port_pad_init runs regardless and the worker prints the one line a
 * bug report needs, once it has looked:
 *
 *   [pad] XInput slot 0
 *   [pad] DirectInput: Pro Controller (057e:2009, Switch Pro Controller layout)
 *   [pad] none
 */
#ifndef PORT_PAD_BACKEND_H
#define PORT_PAD_BACKEND_H

#include "hal/host_settings.h"   /* HostPadLayout, the learned layout's shape */

/* Identical in layout to XINPUT_GAMEPAD behind a packet number, i.e. to the
   XINPUT_STATE XInputGetState fills, and to walk_window.cpp's XPad. */
struct PortPadState {
    unsigned long packet;
    unsigned short buttons;     /* the XINPUT_GAMEPAD_* mask */
    unsigned char lt, rt;       /* 0..255 */
    short lx, ly, rx, ry;       /* +-32767, Y positive up */
};

/* Read SM64DS_PAD_BACKEND and start the worker that loads the backends and
   looks for a pad; the worker prints the "[pad] ..." line when it has looked.
   Returns nonzero unless the pad is forced off or the worker could not start;
   a zero return means no poll can ever succeed. Does not block. Safe to call
   more than once; only the first call does anything. */
int port_pad_init(void);

/* Tell the worker a device came or went (WM_DEVICECHANGE), so the next scan
   happens now rather than on the slow cadence. Cheap; any thread. */
void port_pad_device_changed(void);

/* Read the current pad. Returns 1 and fills *out when a pad is connected and
   answering; returns 0 and leaves *out untouched otherwise, which is exactly
   what XInputGetState(0, &pad) == 0 used to mean. */
int port_pad_poll(PortPadState *out);

/* The backend and device the last successful discovery named, for a log or
   an overlay. "none" until something answers. */
const char *port_pad_describe(void);

/* ---- the learn flow's raw view ------------------------------------------
   The DirectInput pad's state BEFORE any layout: which button indices are
   down and where each of the six axes sits, plus what the pad is. live is 1
   only when a DirectInput pad is bound AND it is the pad port_pad_poll
   answers with (an XInput slot that answers wins, and XInput layouts need no
   learning). Axes are scaled to +-32767 in DirectInput's own sense (Y grows
   downward), 0 when the pad lacks the axis. Cheap: one copy under the lock. */
struct PortPadRaw {
    int live;
    int learned;                /* the layout in effect is a learned one */
    unsigned vid, pid;
    unsigned buttons;           /* bit i: DirectInput button i is down, i < 32 */
    short axis[6];              /* X Y Z Rx Ry Rz */
    unsigned char present[6];   /* the pad has that axis */
    char name[96];              /* the product name DirectInput gave */
};
int port_pad_raw(PortPadRaw *out);

/* Install (or replace) the layout for one vid:pid. Beats the built-in table
   from the worker's next pass on, so a pad already bound answers with it at
   once. Does not touch settings.json; the caller saves through
   host_setting_save_pad_layout if it wants the layout back next run. At most
   HOST_PAD_LAYOUT_MAX pads; a further one is refused (returns 0). */
int port_pad_set_layout(const HostPadLayout *layout);

/* The translation checked on a synthetic report: a learned layout is
   installed for a made-up vid:pid, a hand-built joystick state is pushed
   through the same code the worker runs, and the XInput-shaped answer is
   compared bit for bit. Returns 1 on pass and says what failed on stderr.
   FOR A TEST PROCESS ONLY, before port_pad_init: it writes the worker's own
   statics. */
int port_pad_selftest(void);

#endif /* PORT_PAD_BACKEND_H */
