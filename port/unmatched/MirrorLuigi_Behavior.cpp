/* HOST COPY of src/_ZN11MirrorLuigi8BehaviorEv.cpp -- MirrorLuigi's Behavior,
 * the TICK half of its one-cell pointer-to-member state machine (id 195, ov055,
 * level 47). Run rel0215, lane cast-sweep2.
 *
 * WHY A HOST COPY. The matched src runs the tick dispatch INLINE, so the whole
 * TU has to be the host body rather than a one-line forwarder:
 *
 *     struct C5; typedef void (C5::*PMF)(char *);
 *     struct Node { int pad0, pad1; PMF fn; };
 *     node = *(Node **)((char *)&unk_1d8);
 *     if (*(int *)&node->fn != 0) (((C5 *)this)->*node->fn)(val);
 *
 * `C5` is never defined, so mwcc's PMF is the ROM's 8-byte {function, delta}
 * pair while MSVC's is the "unknown inheritance" form. The two int pads step
 * over the cell's ENTER half to reach the TICK half at cell+8, which is the
 * Ukiki `c->pp + 1` step written a third way. The full reasoning is in
 * port/unmatched/MirrorLuigi_StateEnter.cpp's header and is not repeated.
 *
 * ROM 0x02111594, the dispatch:
 *     ldr  r1, [r5, #0x1d8]      the cell
 *     ldr  r0, [r1, #8]          the TICK half's function word
 *     cmp  r0, #0 -> skip
 *     add  r3, r1, #8
 *     ldr  r1, [r3, #4]          the TICK half's delta word
 *     add  r0, r5, r1, asr #1    the receiver
 *     ands r1, r1, #1            bit 0 selects a VIRTUAL call
 *     ldrne r2, [r0] / ldrne r1, [r3] / ldrne r2, [r2, r1]
 *     mov  r1, r4                the player pointer, passed through
 *     ldreq r2, [r3]
 *     blx  r2
 *
 * WHAT THE TICK DOES is the class's whole point: func_ov055_02111288 copies the
 * player's position with the X negated and the Y angle negated, which is what
 * makes this a MIRROR Luigi. Everything else below is the matched source
 * statement for statement.
 *
 * The matched src TU stays in src/ as the byte proof and is dropped from
 * port/slice_sweep2_ov055.txt. It is a real C++ method there
 * (`int MirrorLuigi::Behavior()`), so this file exports the Itanium name
 * directly rather than being faced -- the Wiggler_Render.cpp shape.
 *
 * PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
 * class; MSVC's PMF representation there does not reproduce the ROM's
 * {function, delta} pair.
 */
#include "MirrorLuigi.h"

extern "C" {
extern int data_0209caa0[];
extern unsigned char data_0209f250;
extern char *data_0209f394[];
extern char *data_0209f318;
extern int data_ov055_02111a90;
extern int data_ov055_02111b60;
extern int data_ov055_02111b64;
extern int data_ov055_02111b68;
extern int data_ov055_02111b6c;
void _Z14ApproachLinearRiii(int *r, int target, int step);
void Matrix4x3_FromTranslation(char *m, int x, int y, int z);
void func_ov002_020e4374(char *c, int *p1, int *p2);
void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    char *a, char *sh, char *m, int height, int rad, unsigned int frames);

struct PortMirrorLuigiCell { unsigned enter_fn, enter_delta, tick_fn, tick_delta; };
typedef void (*PortMirrorLuigiTickFn)(void *, void *);

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch on a deliberately incomplete
 * class. */
int _ZN11MirrorLuigi8BehaviorEv(void *selfv)
{
    char *self = (char *)selfv;
    int a, b;
    char *val;

    int f = data_ov055_02111b68;
    if (f & 1) {
        data_ov055_02111b6c = 0;
        data_ov055_02111a90 = 0x1ffff;
        data_ov055_02111b60 = 0;
    } else if ((f & 2) && !(data_0209caa0[1] & 0x10)) {
        _Z14ApproachLinearRiii(&data_ov055_02111b6c, 0x1ffff, 0x400);
        data_ov055_02111b64 = (data_ov055_02111b64 & ~0x20000)
                            + (0x1ffff - data_ov055_02111b6c);
    }

    val = data_0209f394[data_0209f250];

    {
        PortMirrorLuigiCell *cell = *(PortMirrorLuigiCell **)(self + 0x1d8);
        if (cell->tick_fn != 0) {
            char *recv = self + ((int)cell->tick_delta >> 1);
            PortMirrorLuigiTickFn fn;
            if (cell->tick_delta & 1)
                fn = (PortMirrorLuigiTickFn)(size_t)
                     (*(unsigned **)recv)[cell->tick_fn / 4];
            else
                fn = (PortMirrorLuigiTickFn)(size_t)cell->tick_fn;
            fn(recv, val);
        }
    }

    Matrix4x3_FromTranslation(self + 0x1dc,
                              *(int *)(self + 0x5c) >> 3,
                              *(int *)(self + 0x60) >> 3,
                              *(int *)(self + 0x64) >> 3);
    func_ov002_020e4374(val, &a, &b);
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        self, self + 0x188, self + 0x1dc, b, a, 0xf);
    *(char **)(data_0209f318 + 0x114) = self;
    return 1;
}
}
