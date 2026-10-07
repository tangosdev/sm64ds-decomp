#ifndef DSCMGPACHINKO2_C_H
#define DSCMGPACHINKO2_C_H
#include "dScMgBase_c.h"

/* The Tamaire minigame scene: scene 0x171, profile MG_TAMAIRE. RTTI derives
 * dScMgPachinko2_c directly from dScMgBase_c, and no RTTI record names it as
 * a base. It overrides vtable slots 0 (InitResources), 6 (Behavior),
 * 9 (Render), 16 and 17 (the destructors) and 18 (OnYoshiTryEat). Offsets
 * below 0x4660 belong to dScMgBase_c. Own offsets that only the unnamed
 * helper functions touch are still padding.
 *
 * The factory dScMgPachinko2_c_classInit (historical alias
 * MgLakituLaunch_Spawn) installs this class's vtable. The ROM proves the
 * class; the classInit spelling follows later EAD lineage.
 */
/* One launched ball. func_ov006_02102fe8 drives it from the touch record while
   the pen is down (x/y follow the pen, px/py keep the pen offset) and, on
   release, aims it at (0x80, 0x20): angle = atan2, speed from the distance,
   vx/vy from the sine table. */
struct dScMgPachinko2_Ball {
    s32 x;            /* +0x00 -- Fix12 */
    s32 y;            /* +0x04 */
    s32 vx;           /* +0x08 */
    s32 vy;           /* +0x0c */
    s32 px;           /* +0x10 -- pen offset while held */
    s32 py;           /* +0x14 */
    u8  unk_18[0x8];
    s32 speed;        /* +0x20 */
    u8  unk_24[0x4];
    s32 sound;        /* +0x28 -- Sound_PlayIfNotActive handle */
    s32 prevDist;     /* +0x2c */
    u8  unk_30[0x2];
    s16 unk_32;       /* +0x32 */
    s16 angle;        /* +0x34 */
    s16 unk_36;       /* +0x36 */
    u8  unk_38;
    u8  state;        /* +0x39 -- 2 while released, 0 when reset */
    u8  unk_3a[0x6];
};
#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgPachinko2_Ball_size_must_be_0x40[sizeof(struct dScMgPachinko2_Ball) == 0x40 ? 1 : -1];
#endif

struct dScMgPachinko2_c : dScMgBase_c {
    virtual ~dScMgPachinko2_c();  /* slots 16 and 17 */
    virtual s32 InitResources();  /* slot 0 */
    virtual s32 Behavior();       /* slot 6 */
    virtual s32 Render();         /* slot 9 */
    virtual void OnYoshiTryEat(int arg); /* slot 18 */

    /* cups: the two 0x20-byte drop targets at 0x5620 (the mBalls[63] slot) */
    void func_ov006_020ff47c();            /* render both cups */
    void func_ov006_020ff4ec();            /* reset cup slots */
    void func_ov006_020ff534(int k);       /* launched-ball vs cup k check */
    void func_ov006_020ff690(int k);       /* cup state 4: randomize */
    void func_ov006_020ff8c8(int k);       /* cup state 3 */
    void func_ov006_020ffb54(int k);       /* cup state 2 */
    void func_ov006_020ffde4(int k);       /* cup state 1 */
    void func_ov006_020fff54(int k);       /* cup state 0: idle reset */
    void func_ov006_020fff84();            /* cup PMF dispatch, data_ov006_021426cc */
    void func_ov006_020fffec();            /* init both cups */
    void func_ov006_02100058();            /* clear cup flags */

    /* scene state */
    void func_ov006_02100084();            /* pick lane config -> unk_5668 */
    void func_ov006_02100488();            /* round-end jingle gate */
    void func_ov006_021004c0();            /* round-end sfx gate */
    void func_ov006_021004f4(int a);       /* enter state 3, fade */
    void func_ov006_02100554();            /* all-paddles-done check -> state 3 */
    void func_ov006_021024e0();            /* arm paddles for the round */
    void func_ov006_0210258c();            /* enter state 2 */
    void func_ov006_02102624();            /* countdown tick gate */
    void func_ov006_0210265c();            /* countdown ticker */
    void func_ov006_02102718();            /* cancel launch if ball left pen */
    void func_ov006_021027e4(int a1, int a2, int a3); /* BG2 tile stamp (a0 unused) */
    void func_ov006_02102864();            /* draw the guide lines */
    void func_ov006_02102ef4();            /* arm 30-tick countdown */
    void func_ov006_02103ac0();            /* launch queued ball */
    void func_ov006_02103bfc();            /* reset the whole round */

    /* sprite bursts: the two 16 x 0x18 records at 0x5320 and 0x54a0 */
    void func_ov006_02100140();            /* render records at 0x54a0 */
    void func_ov006_021001ac();            /* step records at 0x54a0 */
    void func_ov006_02100278(int r1, int r2, int r3); /* spawn at 0x54a0 */
    void func_ov006_02100314();            /* render records at 0x5320 */
    void func_ov006_02100380();            /* step records at 0x5320 */
    void func_ov006_02100408(int a2, int a3);         /* spawn at 0x5320 */

    /* paddles: the three 0x40-byte records at 0x5260 */
    void func_ov006_0210068c();            /* render paddles */
    void func_ov006_021006f4();            /* reset paddle timers */
    void func_ov006_02100734(int idx);     /* park paddle idx */
    void func_ov006_0210076c(int idx);     /* paddle idx vs balls */
    void func_ov006_0210246c();            /* paddle PMF dispatch, data_ov006_02142734 */
    void func_ov006_02102564();            /* clear paddle flags */
    void func_ov006_021009b8(int i);       /* paddle state 14 */
    void func_ov006_02100b08(int i);       /* paddle state 13 */
    void func_ov006_02100bac(int i);       /* paddle state 12: seek lane target */
    void func_ov006_02100d90(int i);       /* paddle state 11 */
    void func_ov006_02100e3c(int i);       /* paddle state 10 */
    void func_ov006_02100f7c(int i);       /* paddle state 9 */
    void func_ov006_02101088(int i);       /* paddle state 8 */
    void func_ov006_02101148(int i);       /* collect -> next state */
    void func_ov006_02101224(int i);       /* paddle state 7 */
    void func_ov006_021012cc(int i);       /* paddle state 6: horizontal patrol */
    void func_ov006_021016ec(int i);       /* paddle state 5 */
    void func_ov006_021019e0(int i);       /* paddle state 4: circle lane */
    void func_ov006_02101af0(int i);       /* paddle state 3 */
    void func_ov006_02101e88(int i);       /* paddle state 2 */
    void func_ov006_021020c4(int i);       /* paddle state 1: rise then split */
    void func_ov006_02102274(int i);       /* paddle state 0: enter */

    /* balls: mBalls[0x30] active */
    void func_ov006_02102c3c(int x, int z, int d); /* spawn ball */
    void func_ov006_02102d6c(int i);       /* ball i hits the floor */
    void func_ov006_02102dbc();            /* clear ball flags */
    void func_ov006_02102de4();            /* render balls */
    void func_ov006_02102e8c();            /* ball PMF dispatch, data_ov006_021426f4 */
    void func_ov006_02102f3c(int arg1);    /* ball state 0: idle in tray */
    void func_ov006_02102fe8(int i);       /* ball state 1: pen drag */
    void func_ov006_02103360(int i);       /* ball state 2: launched flight */
    void func_ov006_02103608(int i);       /* ball state 3 */
    void func_ov006_0210371c(int i);       /* ball state 4 */
    void func_ov006_02103870(int i);       /* ball state 5 */
    void func_ov006_0210397c(int i);       /* ball state 6: despawn */
    void func_ov006_02103994(int i);       /* ball state 7 */

    dScMgPachinko2_Ball mBalls[0x40]; /* 0x4660 -- 0x40 x 0x40; see func_ov006_02102fe8 */
    s32 unk_5660;            /* 0x5660 -- Behavior switches on it, 0..3 */
    u8  pad_5664[0x8];
    u16 unk_566c;            /* 0x566c */
    u16 unk_566e;            /* 0x566e -- set to 0x40; Behavior's case 3 counts it down */
    /* the factory allocates 0x567c bytes; see tools/opnew_sizes.py */
    u8 pad_5670[0xc];
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgPachinko2_c_size_must_be_0x567c[sizeof(struct dScMgPachinko2_c) == 0x567c ? 1 : -1];
#endif

#endif
