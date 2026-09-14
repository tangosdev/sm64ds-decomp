/* ov063 0x02117cdc, 0x77c bytes -- the long half of the same NPC talk state
 * machine src/unnamed/ov063/func_ov063_02117b0c.c drives, on the same 0x5ce
 * state byte and the same 0x488 "the player we are talking to" slot. States 0-2
 * acquire the closest player, wait out the 0x100 timer and open the
 * conversation; 3 turns the actor and the camera onto it; 4-6 run the
 * SlidingIce teardown and two more timer waits; 7-8 sweep the camera back and
 * ramp the 0x5c8 fade byte to 0xff; 9 shows the message; 10 closes it, clears
 * flag bit 0 of 0x19c and bit 3 of the camera's 0x154, seeds the 0x5be and
 * 0x5c4 timers from RandomIntInternal and starts music 0x2d.
 *
 * TWO SPELLINGS IN THE FIRST FIVE LINES DECIDE THE ENTRY BLOCK'S THREE WORDS.
 * All 479 words are otherwise fixed; only `ldrb`/`ldr`, the `cmp` and the
 * `addls pc` move, and they move together. Measured at 2004/b56:
 *
 *   ClosestPlayer()  + wide-launder read    ldrb r0 / ldr r1   3 words off
 *   ClosestPlayer()  + plain read           ldrb r1 / ldr r0   5 words off
 *   ClosestPlayer(c) + wide-launder read    ldrb r1 / ldr r2   5 words off
 *   ClosestPlayer(c) + plain read           ldrb r2 / ldr r1   the cartridge
 *
 * "plain read" is `gp = data_0209f318;` below; "wide-launder read" is the
 * banked draft's, which put `&data_0209f318` through a `(long long)(int)` cast
 * and a 64-bit identity mask before dereferencing it.
 *
 * _ZN8dActor_c13ClosestPlayerEv is dActor_c::ClosestPlayer(), a non-static
 * method, so the actor rides in r0 -- and because that call is the first thing
 * case 0 does, mwccarm coalesces the argument with the incoming parameter and
 * emits no `mov r0, r6` at all. The call site is byte-identical either way (see
 * a6894f41b, which fixed eleven other callers for free), but the coalesce is
 * what keeps the parameter's r0 live across the switch dispatch, so the two
 * entry-block temporaries start at r1 instead of r0. Declaring the method
 * `(void)` retires r0 at the `mov r6, r0` home and costs the three words.
 *
 * Reading the camera pointer straight out of the `char *` global -- the way
 * func_ov063_02117b0c reads it -- creates the pool-address web before the
 * selector's. The wide launder reverses those two, which is the third row
 * above; it was load-bearing while the call was spelled `(void)` and is dead
 * weight once it is not.
 *
 * LAUND is this file's own no-op launder, as in the sibling: it forces the
 * address of a read-modify-write into a register first instead of letting
 * mwccarm re-fold it into the store, the compound-assignment lever
 * notes/mwccarm-codegen.md records. */
#include "types.h"

extern void *_ZN8dActor_c13ClosestPlayerEv(void *thisp);
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

extern char *data_0209f318;
extern int data_ov008_02111b6c;
extern int data_0209caa0[];
extern s16 data_ov063_0211e1dc[];
extern s16 data_ov063_0211e1e4[];
extern int data_0209e650;

#define LAUND(p) ((void *)(p))

void func_ov063_02117cdc(char *c) {
    s16 v[3];
    int w[3];
    char *gp;
    int neg;

    gp = data_0209f318;
    switch (*(u8 *)(c + 0x5ce)) {
    case 0:
        *(void **)(c + 0x488) = _ZN8dActor_c13ClosestPlayerEv(c);
        if (*(int *)(*(int *)(c + 0x488) + 0x5c) <= -0x2bc000) {
            return;
        }
        *(u8 *)LAUND(c + 0x5ce) += 1;
        *(s16 *)(c + 0x100) = 0;
        return;
    case 1: {
        u8 *st;
        int n;
        if (*(u16 *)(c + 0x100) < 0x96) {
            return;
        }
        st = (u8 *)LAUND(c + 0x5ce);
        n = *st + 1;
        *st = n;
        return;
    }
    case 2: {
        int *src;
        int *d54c;
        int *d554;
        if (*(int **)(c + 0x488) == 0) {
            return;
        }
        src = (int *)LAUND((char *)*(int **)(c + 0x488) + 0x5c);
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        if (LenVec3(w) >= 0x12c000) {
            return;
        }
        if (_ZN6Player9StartTalkER7fBase_cb(*(void **)(c + 0x488), c, 1) == 0) {
            return;
        }
        _ZN6Camera9SetFlag_3Ev(gp);
        *(int *)(c + 0x54c) = w[0];
        *(int *)(c + 0x550) = w[1];
        *(int *)(c + 0x554) = w[2];
        d54c = (int *)LAUND(c + 0x54c);
        d554 = (int *)LAUND(c + 0x554);
        *d54c = *d54c - (0x3c000 - (w[0] / 8));
        *(int *)(c + 0x550) = 0x64000;
        *d554 = *d554 - (0x64000 - (w[0] / 6));
        *(int *)(c + 0x558) = w[0];
        *(int *)(c + 0x55c) = w[1];
        *(int *)(c + 0x560) = w[2];
        *(int *)(c + 0x558) = 0;
        *(int *)(c + 0x55c) = 0x64000;
        {
            u8 *st = (u8 *)LAUND(c + 0x5ce);
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
        /* LAU forces add+ldr materialisation (not ldr [rN,#0x5c] first) */
        src = (int *)LAUND((char *)*(void **)(c + 0x488) + 0x5c);
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        w[0] = 0;
        ang = Vec3_HorzAngle((int *)((char *)*(void **)(c + 0x488) + 0x5c), w);
        q = (s16 *)LAUND((char *)*(void **)(c + 0x488) + 0x8c);
        v[0] = q[0];
        v[1] = q[1];
        v[2] = q[2];
        b = func_020092c4(gp, gp + 0x8c, c + 0x54c);
        b = b & func_020092c4(gp, gp + 0x80, c + 0x558);
        if (_Z14ApproachLinearRsss(&v[1], ang, 0x200) != 0 && b != 0) {
            *(u8 *)LAUND(c + 0x5ce) += 1;
            *(s16 *)(c + 0x100) = 0;
        }
        {
            char *d = (char *)*(void **)(c + 0x488);
            *(s16 *)(d + 0x8c) = v[0];
            *(s16 *)(d + 0x8e) = v[1];
            *(s16 *)(d + 0x90) = v[2];
        }
        {
            char *d = (char *)*(void **)(c + 0x488);
            *(s16 *)(d + 0x92) = v[0];
            *(s16 *)(d + 0x94) = v[1];
            *(s16 *)(d + 0x96) = v[2];
        }
        return;
    }
    case 4:
        if (*(u16 *)(c + 0x100) < 0x1e) {
            return;
        }
        _ZN10SlidingIce16CleanupResourcesEv();
        {
            u8 *st = (u8 *)LAUND(c + 0x5ce);
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
        src = (int *)LAUND((char *)*(void **)(c + 0x488) + 0x5c);
        d560 = (int *)LAUND(c + 0x560);
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        *d560 = *d560 + (0x50000 - (w[0] / 3));
        *(int *)(c + 0x5c) = w[0] - 0xc8000;
        *(int *)(c + 0x64) = (*(int *)(c + 0x560) + 0x12c000) - ((w[0] * 2) / 3);
        *(int *)(c + 0x54c) = w[0] - 0x82000;
        *(int *)(c + 0x554) = w[2] - 0x32000;
        *(s16 *)(c + 0x100) = 0;
        {
            u8 *st2 = (u8 *)LAUND(c + 0x5ce);
            int n = *st2 + 1;
            *st2 = n;
        }
        return;
    }
    case 6:
        if (*(u16 *)(c + 0x100) < 0x1e) {
            return;
        }
        {
            u8 *st = (u8 *)LAUND(c + 0x5ce);
            int n = *st + 1;
            *st = n;
        }
        return;
    case 7: {
        int *src;
        int t0;
        t0 = func_020092c4(gp, gp + 0x8c, c + 0x54c);
        if ((t0 & func_020092c4(gp, gp + 0x80, c + 0x558)) == 0) {
            return;
        }
        *(u8 *)(c + 0x5c8) = 0;
        *(u8 *)LAUND(c + 0x5ce) += 1;
        neg = -1;
        src = (int *)LAUND((char *)*(void **)(c + 0x488) + 0x5c);
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        w[0] = w[0] * neg;
        *(s16 *)(c + 0x8e) = Vec3_HorzAngle((int *)(c + 0x5c), w);
        *(s16 *)(c + 0x94) = *(s16 *)(c + 0x8e);
        func_0201277c(0x150);
        return;
    }
    case 8:
        /* invert so ELSE (=0xff) is predicated and THEN (RMW) is branched (codegen 6c) */
        if (*(u8 *)(c + 0x5c8) + 5 < 0xff) {
            u8 *p = (u8 *)LAUND(c + 0x5c8);
            *p = (u8)(*p + 5);
        } else {
            *(u8 *)(c + 0x5c8) = 0xff;
        }
        w[0] = *(int *)(c + 0x5c);
        w[1] = *(int *)(c + 0x60);
        w[2] = *(int *)(c + 0x64);
        neg = -1;
        w[0] = w[0] * neg;
        w[1] = w[1] + 0xc8000;
        func_ov002_020c51d0(*(void **)(c + 0x488), w);
        if (*(u8 *)(c + 0x5c8) == 0xff) {
            *(u8 *)LAUND(c + 0x5ce) += 1;
            *(u8 *)(c + 0x5c9) = 0xff;
        }
        *(s16 *)(c + 0x8e) = *(s16 *)(c + 0x94);
        return;
    case 9: {
        int tk = _ZN6Player12GetTalkStateEv(*(void **)(c + 0x488));
        s16 msg;
        if (tk != 0) {
            return;
        }

        {
            int x = 0 - *(int *)(c + 0x5c);
            int z = *(int *)(c + 0x64);
            int y = *(int *)(c + 0x60) + 0xc8000;
            int fl = data_0209caa0[1] & 0x10;
            w[0] = x;
            w[1] = y;
            w[2] = z;
            if (fl)
                msg = data_ov063_0211e1e4[*(int *)(*(int *)(c + 0x488) + 8)];
            else
                msg = data_ov063_0211e1dc[*(int *)(*(int *)(c + 0x488) + 8)];
        }
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void **)(c + 0x484), c, (u32)msg, w, 0, 2) == 0) {
            return;
        }
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2c);
        {
            u8 *st = (u8 *)LAUND(c + 0x5ce);
            int n = *st + 1;
            *st = n;
        }
        return;
    }
    case 10: {
        int tk = _ZN6Player12GetTalkStateEv(*(void **)(c + 0x488));
        u32 rr;
        if (tk != -1) {
            return;
        }
        *(u8 *)(c + 0x5cc) = 1;
        *(int *)LAUND(c + 0x19c) &= ~1;
        *(s16 *)(c + 0x5be) = (((u32)RandomIntInternal(&data_0209e650) >> 0x10) & 0x3f) + 0xb4;
        *(int *)LAUND(gp + 0x154) &= ~8;
        rr = RandomIntInternal(&data_0209e650);
        *(s16 *)(c + 0x5c4) = ((rr >> 0x10) % 0x96) + 0x12c;
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
        func_02011d14();
        return;
    }
    }
    return;
}
