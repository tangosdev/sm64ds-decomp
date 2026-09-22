/* HOST COPY of func_ov063_02117cdc (ov063 0x02117cdc, 0x77c bytes), the Boo
 * TALK/CUTSCENE sub-state machine (11 sub-states on +0x5ce: approach, camera
 * grab through func_020092c4, Player::StartTalk, the fade-out on +0x5c8, the
 * ShowMessage with the id picked from data_ov063_0211e1dc/0211e1e4, music
 * layers 0x2c/0x2d). Reached from Boo state 15 (func_ov063_021172a8, on the
 * slice). NOT matched in src/.
 *
 * PROVENANCE: the near-miss DB draft for 0x02117cdc, verified with
 * tools/abverify.py: equal size (479/479 words), 11 divergences, all
 * register renames (r2/r1, r0/r2, r4) plus one str/ands order swap -- no
 * missing call, no missing store. Verified source + diff in
 * runs/linkw/out/w5a/evidence/hostcopy_verified/. THREE HOST EDITS on top of
 * the verified text, each ROM-derived:
 *
 * 1. RECEIVER FIX (the Actor_ClosestPlayer_OverlayReaders.cpp seam): case 0
 *    calls ClosestPlayer with no argument; the ROM leaves the Boo in r0
 *    (measured at 0x02117d2c: nothing touches r0 between entry and the bl).
 *    The host call passes arg0 explicitly.
 * 2. WINDOW WORD (case 5): the ROM reads *(int*)0x02111b6c -- an address in
 *    the LEVEL-overlay load window, so its content depends on which level
 *    overlay is loaded. On level 12 that is ov020's image and the word is
 *    0x112fff1e (an instruction of func_ov020_02111b28, measured from
 *    overlay_0020.bin), so the ==0x1f000 test is FALSE and the input-flag
 *    arm decides. ov63_seq_window_word() reads the mounted ov020 image on
 *    level 12 and returns 0 (equally !=0x1f000) elsewhere, with a one-shot
 *    stderr note. Whichever level the 0x1f000 sentinel was written against
 *    is NOT identified yet -- the courtyard's ov011 is only 0x5c0 bytes and
 *    ends short of this address, so on level 3 the ROM reads bytes of
 *    whatever overlay occupied the window before ov011 (stale-memory read on
 *    real hardware). Handoff: identify the sentinel's home before mounting
 *    the cutscene on any other level.
 * 3. WINDOW CALL (case 4): the ROM calls 0x02111264 in the same window. On
 *    level 12 that address is MID-FUNCTION ov020 bytes (no symbol starts
 *    there; the word is 0xebfc0017, the tail of another body's code), so
 *    this path cannot legitimately run on level 12 -- if it ever fires the
 *    ROM would have jumped into the middle of an instruction stream too.
 *    ov63_seq_window_call() declines loudly (stderr, once) and advances the
 *    state machine exactly as the ROM's return would. Five overlays carry a
 *    real function AT 0x02111264 (ov027/ov040/ov042/ov048/ov055, config
 *    sweep); which of them the cutscene was written against is the same
 *    handoff.
 */
#include <stdio.h>

extern unsigned char port_ov020_image[];
extern int port_level_id(void);

static int ov63_seq_window_word(void)
{
    if (port_level_id() == 12)
        return *(int *)(port_ov020_image + (0x02111b6c - 0x021111a0));
    {
        static int said;
        if (!said) {
            said = 1;
            fprintf(stderr, "UNHOSTED: boo-talk window word 0x02111b6c read "
                    "outside level 12 (level %d); returning 0\n",
                    port_level_id());
        }
    }
    return 0;
}

static void ov63_seq_window_call(void)
{
    static int said;
    if (!said) {
        said = 1;
        fprintf(stderr, "UNHOSTED: boo-talk window call 0x02111264 (level %d) "
                "-- level-overlay cutscene body not hosted, declined\n",
                port_level_id());
    }
}

typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;
typedef signed int s32;

extern void *_ZN8dActor_c13ClosestPlayerEv(void *self);   /* host edit 1: receiver */
extern int LenVec3(int *v);
extern u16 _ZN6Player9StartTalkER7fBase_cb(void *p, void *a, int b);
extern void _ZN6Camera9SetFlag_3Ev(void *c);
extern s16 Vec3_HorzAngle(const int *v0, const int *v1);
extern int func_020092c4(void *a, void *out, void *target);
extern int _Z14ApproachLinearRsss(s16 *a, s16 b, s16 c);
extern u16 func_0201277c(int a);
extern int func_ov002_020c51d0(void *c, int *st);
extern int _ZN6Player12GetTalkStateEv(void *p);
extern u16 _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *a, void *b, u32 c, const int *d, u32 e, u32 f);
extern void _ZN5Sound22LoadAndSetMusic_Layer3Ej(u32 x);
extern int RandomIntInternal(int *seed);
extern u16 func_02011d14(void);

typedef struct { int x, y, z; } Vec3;

extern void *data_0209f318;
extern int data_0209caa0[];
extern s16 data_ov063_0211e1dc[];
extern s16 data_ov063_0211e1e4[];
extern int data_0209e650;

#define LAU(p) ((void *)((long long)(int)(p) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov063_02117cdc(u8 *arg0) {
    s16 v[3];
    int w[3];
    void *r5;
    int neg;

    r5 = (&data_0209f318)[0];
    switch (arg0[0x5ce]) {
    case 0:
        *(void **)(arg0 + 0x488) = _ZN8dActor_c13ClosestPlayerEv(arg0);  /* host edit 1 */
        if (*(int *)(*(int *)(arg0 + 0x488) + 0x5c) <= -0x2bc000) {
            return;
        }
        *(u8 *)LAU(arg0 + 0x5ce) += 1;
        *(s16 *)(arg0 + 0x100) = 0;
        return;
    case 1: {
        u8 *st;
        int n;
        if (*(u16 *)(arg0 + 0x100) < 0x96) {
            return;
        }
        st = (u8 *)LAU(arg0 + 0x5ce);
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
        src = (Vec3 *)LAU((char *)*(int **)(arg0 + 0x488) + 0x5c);
        w[0] = src->x;
        w[1] = src->y;
        w[2] = src->z;
        if (LenVec3(w) >= 0x12c000) {
            return;
        }
        if (_ZN6Player9StartTalkER7fBase_cb(*(void **)(arg0 + 0x488), arg0, 1) == 0) {
            return;
        }
        _ZN6Camera9SetFlag_3Ev(r5);
        *(int *)(arg0 + 0x54c) = w[0];
        *(int *)(arg0 + 0x550) = w[1];
        *(int *)(arg0 + 0x554) = w[2];
        d54c = (int *)LAU(arg0 + 0x54c);
        d554 = (int *)LAU(arg0 + 0x554);
        *d54c = *d54c - (0x3c000 - (w[0] / 8));
        *(int *)(arg0 + 0x550) = 0x64000;
        *d554 = *d554 - (0x64000 - (w[0] / 6));
        *(int *)(arg0 + 0x558) = w[0];
        *(int *)(arg0 + 0x55c) = w[1];
        *(int *)(arg0 + 0x560) = w[2];
        *(int *)(arg0 + 0x558) = 0;
        *(int *)(arg0 + 0x55c) = 0x64000;
        {
            u8 *st = (u8 *)LAU(arg0 + 0x5ce);
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
        src = (int *)LAU((char *)*(void **)(arg0 + 0x488) + 0x5c);
        w[0] = src[0];
        w[1] = src[1];
        w[2] = src[2];
        w[0] = 0;
        ang = Vec3_HorzAngle((int *)((char *)*(void **)(arg0 + 0x488) + 0x5c), w);
        q = (s16 *)LAU((char *)*(void **)(arg0 + 0x488) + 0x8c);
        v[0] = q[0];
        v[1] = q[1];
        v[2] = q[2];
        b = func_020092c4(r5, (char *)r5 + 0x8c, arg0 + 0x54c);
        b = b & func_020092c4(r5, (char *)r5 + 0x80, arg0 + 0x558);
        if (_Z14ApproachLinearRsss(&v[1], ang, 0x200) != 0 && b != 0) {
            *(u8 *)LAU(arg0 + 0x5ce) += 1;
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
        ov63_seq_window_call();  /* host edit 3 */
        {
            u8 *st = (u8 *)LAU(arg0 + 0x5ce);
            int n = *st + 1;
            *st = n;
        }
        return;
    case 5: {
        int *src;
        int *d560;
        u8 *st;
        int v0;
        if (ov63_seq_window_word() == 0x1f000 || (data_0209caa0[1] & 0x10)) {  /* host edit 2 */
            v0 = 1;
        } else {
            v0 = 0;
        }
        if (v0 == 0) {
            if ((data_0209caa0[1] & 0x10) == 0) {
                return;
            }
        }
        src = (int *)LAU((char *)*(void **)(arg0 + 0x488) + 0x5c);
        d560 = (int *)LAU(arg0 + 0x560);
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
            u8 *st2 = (u8 *)LAU(arg0 + 0x5ce);
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
            u8 *st = (u8 *)LAU(arg0 + 0x5ce);
            int n = *st + 1;
            *st = n;
        }
        return;
    case 7: {
        int *src;
        int t0;
        t0 = func_020092c4(r5, (char *)r5 + 0x8c, arg0 + 0x54c);
        if ((t0 & func_020092c4(r5, (char *)r5 + 0x80, arg0 + 0x558)) == 0) {
            return;
        }
        arg0[0x5c8] = 0;
        *(u8 *)LAU(arg0 + 0x5ce) += 1;
        neg = -1;
        src = (int *)LAU((char *)*(void **)(arg0 + 0x488) + 0x5c);
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
            u8 *p = (u8 *)LAU(arg0 + 0x5c8);
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
            *(u8 *)LAU(arg0 + 0x5ce) += 1;
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
            int *caa = data_0209caa0;
            int y = *(int *)(arg0 + 0x60);
            int x = *(int *)(arg0 + 0x5c);
            int z = *(int *)(arg0 + 0x64);
            int fl = caa[1];
            /* pin load order via b?a:a chain */
            y = x ? y : y;
            x = z ? x : x;
            z = fl ? z : z;
            y = y + 0xc8000;
            x = 0 - x;
            fl = fl & 0x10;
            w[1] = y;
            w[2] = z;
            w[0] = x;
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
            u8 *st = (u8 *)LAU(arg0 + 0x5ce);
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
        *(int *)LAU(arg0 + 0x19c) &= ~1;
        *(s16 *)(arg0 + 0x5be) = (((u32)RandomIntInternal(&data_0209e650) >> 0x10) & 0x3f) + 0xb4;
        *(int *)LAU((char *)r5 + 0x154) &= ~8;
        rr = RandomIntInternal(&data_0209e650);
        *(s16 *)(arg0 + 0x5c4) = ((rr >> 0x10) % 0x96) + 0x12c;
        _ZN5Sound22LoadAndSetMusic_Layer3Ej(0x2d);
        func_02011d14();
        return;
    }
    }
    return;
}
