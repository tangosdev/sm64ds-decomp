// @symbol func_ov063_02117cdc
/* recovered: state 7 of the ov063 talk/cutscene actor (dispatched from
 * func_ov063_021172a8 case 7): an eleven-state sequence that finds the closest
 * player, walks up, turns to face them, parks the camera on two anchors, shows
 * the message, then hands control back with a random idle timer.
 *
 * One spelling is load-bearing for the entry block's register colouring
 * (notes/mwccarm-codegen.md 6cx): dActor_c::ClosestPlayer() is a method and
 * takes `this` in r0, and case 0 calls it before anything else clobbers r0, so
 * the incoming parameter's r0 stays live from the entry into that call. That
 * keeps both entry-block temps (the switch selector and the camera pool
 * address) off r0, which is where the ROM has them: ldrb r2 / ldr r1. Declared
 * with no argument, the parameter dies at the homing copy and the selector
 * takes r0.
 */
#include "types.h"

extern void *_ZN8dActor_c13ClosestPlayerEv(void *self); /* dActor_c::ClosestPlayer(), this in r0 */
extern int LenVec3(int *v);
extern u16 _ZN6Player9StartTalkER7fBase_cb(void *p, void *a, int b);
extern void _ZN6Camera9SetFlag_3Ev(void *c);
extern s16 Vec3_HorzAngle(const int *v0, const int *v1);
extern int func_020092c4(void *a, void *out, void *target);
extern int _Z14ApproachLinearRsss(s16 *a, s16 b, s16 c);
extern void _ZN10SlidingIce16CleanupResourcesEv(void);
extern u16 func_0201277c(int a);
extern int func_ov002_020c51d0(void *c, int *st);
extern int _ZN6Player12GetTalkStateEv(void *p);
extern u16 _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *a, void *b, u32 c, const int *d, u32 e, u32 f);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(u32 x);
extern int RandomIntInternal(int *seed);
extern u16 func_02011d14(void);

typedef struct { int x, y, z; } Vec3;

extern void *data_0209f318; /* the camera */
extern int data_ov008_02111b6c;
extern int data_0209caa0[];
extern s16 data_ov063_0211e1dc[];
extern s16 data_ov063_0211e1e4[];
extern int data_0209e650;


void func_ov063_02117cdc(u8 *arg0) {
    s16 v[3];
    int w[3];
    void *cam;
    int neg;

    cam = data_0209f318;
    switch (arg0[0x5ce]) {
    case 0:
        *(void **)(arg0 + 0x488) = _ZN8dActor_c13ClosestPlayerEv(arg0);
        if (*(int *)(*(int *)(arg0 + 0x488) + 0x5c) <= -0x2bc000) {
            return;
        }
        *(u8 *)(arg0 + 0x5ce) += 1;
        *(s16 *)(arg0 + 0x100) = 0;
        return;
    case 1: {
        u8 *st;
        int n;
        if (*(u16 *)(arg0 + 0x100) < 0x96) {
            return;
        }
        st = (u8 *)(arg0 + 0x5ce);
        n = *st + 1;
        *st = n;
        return;
    }
    case 2: {
        Vec3 *src;
        int *d54c;
        int *d554;
        if (*(int **)(arg0 + 0x488) == 0) {
            return;
        }
        src = (Vec3 *)((char *)*(int **)(arg0 + 0x488) + 0x5c);
        w[0] = src->x;
        w[1] = src->y;
        w[2] = src->z;
        if (LenVec3(w) >= 0x12c000) {
            return;
        }
        if (_ZN6Player9StartTalkER7fBase_cb(*(void **)(arg0 + 0x488), arg0, 1) == 0) {
            return;
        }
        _ZN6Camera9SetFlag_3Ev(cam);
        *(int *)(arg0 + 0x54c) = w[0];
        *(int *)(arg0 + 0x550) = w[1];
        *(int *)(arg0 + 0x554) = w[2];
        d54c = (int *)(arg0 + 0x54c);
        d554 = (int *)(arg0 + 0x554);
        *d54c = *d54c - (0x3c000 - (w[0] / 8));
        *(int *)(arg0 + 0x550) = 0x64000;
        *d554 = *d554 - (0x64000 - (w[0] / 6));
        *(int *)(arg0 + 0x558) = w[0];
        *(int *)(arg0 + 0x55c) = w[1];
        *(int *)(arg0 + 0x560) = w[2];
        *(int *)(arg0 + 0x558) = 0;
        *(int *)(arg0 + 0x55c) = 0x64000;
        {
            u8 *st = (u8 *)(arg0 + 0x5ce);
            int n = *st + 1;
            *st = n;
        }
        return;
    }
    case 3: {
        s16 ang;
        int b;
        int *src;
        s16 *q;
        src = (int *)((char *)*(void **)(arg0 + 0x488) + 0x5c);
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        w[0] = 0;
        ang = Vec3_HorzAngle((int *)((char *)*(void **)(arg0 + 0x488) + 0x5c), w);
        q = (s16 *)((char *)*(void **)(arg0 + 0x488) + 0x8c);
        v[0] = q[0];
        v[1] = q[1];
        v[2] = q[2];
        b = func_020092c4(cam, (char *)cam + 0x8c, arg0 + 0x54c);
        b = b & func_020092c4(cam, (char *)cam + 0x80, arg0 + 0x558);
        if (_Z14ApproachLinearRsss(&v[1], ang, 0x200) != 0 && b != 0) {
            *(u8 *)(arg0 + 0x5ce) += 1;
            *(s16 *)(arg0 + 0x100) = 0;
        }
        {
            char *d = (char *)*(void **)(arg0 + 0x488);
            *(s16 *)(d + 0x8c) = v[0];
            *(s16 *)(d + 0x8e) = v[1];
            *(s16 *)(d + 0x90) = v[2];
        }
        {
            char *d = (char *)*(void **)(arg0 + 0x488);
            *(s16 *)(d + 0x92) = v[0];
            *(s16 *)(d + 0x94) = v[1];
            *(s16 *)(d + 0x96) = v[2];
        }
        return;
    }
    case 4:
        if (*(u16 *)(arg0 + 0x100) < 0x1e) {
            return;
        }
        _ZN10SlidingIce16CleanupResourcesEv();
        {
            u8 *st = (u8 *)(arg0 + 0x5ce);
            int n = *st + 1;
            *st = n;
        }
        return;
    case 5: {
        int *src;
        int *d560;
        u8 *st;
        int v0;
        if ((&data_ov008_02111b6c)[0] == 0x1f000 || (data_0209caa0[1] & 0x10)) {
            v0 = 1;
        } else {
            v0 = 0;
        }
        if (v0 == 0) {
            if ((data_0209caa0[1] & 0x10) == 0) {
                return;
            }
        }
        src = (int *)((char *)*(void **)(arg0 + 0x488) + 0x5c);
        d560 = (int *)(arg0 + 0x560);
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        *d560 = *d560 + (0x50000 - (w[0] / 3));
        *(int *)(arg0 + 0x5c) = w[0] - 0xc8000;
        *(int *)(arg0 + 0x64) = (*(int *)(arg0 + 0x560) + 0x12c000) - ((w[0] * 2) / 3);
        *(int *)(arg0 + 0x54c) = w[0] - 0x82000;
        *(int *)(arg0 + 0x554) = w[2] - 0x32000;
        *(s16 *)(arg0 + 0x100) = 0;
        {
            u8 *st2 = (u8 *)(arg0 + 0x5ce);
            int n = *st2 + 1;
            *st2 = n;
        }
        return;
    }
    case 6:
        if (*(u16 *)(arg0 + 0x100) < 0x1e) {
            return;
        }
        {
            u8 *st = (u8 *)(arg0 + 0x5ce);
            int n = *st + 1;
            *st = n;
        }
        return;
    case 7: {
        int *src;
        int t0;
        t0 = func_020092c4(cam, (char *)cam + 0x8c, arg0 + 0x54c);
        if ((t0 & func_020092c4(cam, (char *)cam + 0x80, arg0 + 0x558)) == 0) {
            return;
        }
        arg0[0x5c8] = 0;
        *(u8 *)(arg0 + 0x5ce) += 1;
        neg = -1;
        src = (int *)((char *)*(void **)(arg0 + 0x488) + 0x5c);
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        w[0] = w[0] * neg;
        *(s16 *)(arg0 + 0x8e) = Vec3_HorzAngle((int *)(arg0 + 0x5c), w);
        *(s16 *)(arg0 + 0x94) = *(s16 *)(arg0 + 0x8e);
        func_0201277c(0x150);
        return;
    }
    case 8:
        /* invert so ELSE (=0xff) is predicated and THEN (RMW) is branched (codegen 6c) */
        if (arg0[0x5c8] + 5 < 0xff) {
            u8 *p = (u8 *)(arg0 + 0x5c8);
            *p = (u8)(*p + 5);
        } else {
            arg0[0x5c8] = 0xff;
        }
        w[0] = *(int *)(arg0 + 0x5c);
        w[1] = *(int *)(arg0 + 0x60);
        w[2] = *(int *)(arg0 + 0x64);
        neg = -1;
        w[0] = w[0] * neg;
        w[1] = w[1] + 0xc8000;
        func_ov002_020c51d0(*(void **)(arg0 + 0x488), w);
        if (arg0[0x5c8] == 0xff) {
            *(u8 *)(arg0 + 0x5ce) += 1;
            arg0[0x5c9] = 0xff;
        }
        *(s16 *)(arg0 + 0x8e) = *(s16 *)(arg0 + 0x94);
        return;
    case 9: {
        int tk = _ZN6Player12GetTalkStateEv(*(void **)(arg0 + 0x488));
        s16 msg;
        if (tk != 0) {
            return;
        }

        {
            int x = 0 - *(int *)(arg0 + 0x5c);
            int z = *(int *)(arg0 + 0x64);
            int y = *(int *)(arg0 + 0x60) + 0xc8000;
            int fl = data_0209caa0[1] & 0x10;
            w[0] = x;
            w[1] = y;
            w[2] = z;
            if (fl)
                msg = data_ov063_0211e1e4[*(int *)(*(int *)(arg0 + 0x488) + 8)];
            else
                msg = data_ov063_0211e1dc[*(int *)(*(int *)(arg0 + 0x488) + 8)];
        }
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void **)(arg0 + 0x484), arg0, (u32)msg, w, 0, 2) == 0) {
            return;
        }
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2c);
        {
            u8 *st = (u8 *)(arg0 + 0x5ce);
            int n = *st + 1;
            *st = n;
        }
        return;
    }
    case 10: {
        int tk = _ZN6Player12GetTalkStateEv(*(void **)(arg0 + 0x488));
        u32 rr;
        if (tk != -1) {
            return;
        }
        arg0[0x5cc] = 1;
        *(int *)(arg0 + 0x19c) &= ~1;
        *(s16 *)(arg0 + 0x5be) = (((u32)RandomIntInternal(&data_0209e650) >> 0x10) & 0x3f) + 0xb4;
        *(int *)((char *)cam + 0x154) &= ~8;
        rr = RandomIntInternal(&data_0209e650);
        *(s16 *)(arg0 + 0x5c4) = ((rr >> 0x10) % 0x96) + 0x12c;
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
        func_02011d14();
        return;
    }
    }
    return;
}
