//cpp
/* dScMgCoin_c -- the coin minigame scene: 33 of its helpers
 * (.text 0x020dbf7c..0x020dd0e0).
 *
 * The two functions on either side, func_ov006_020dbe9c and
 * func_ov006_020dd0e0, are not matched yet, so the destructor, the virtual
 * overrides and the factory live in their own files. This file therefore
 * emits no vtable.
 *
 * Functions run in ROM order under `#pragma defer_codegen off`; do not
 * reorder.
 *
 * Several functions keep their own declarations of the same symbol
 * (func_ov006_020ddeb0, func_ov004_020b0380, RenderOamMainScreen) because
 * their call sites disagree on the types, and the local struct views carry a
 * per-function suffix for the same reason.
 *
 * Blocked: no layout for dScMgCoin_c is recovered, so every function takes
 * the scene as raw bytes, and the helpers and tables are unnamed in
 * symbols.txt. What the code shows of the layout:
 *
 *     0x4ac0  24 coins, 0x18 each: position, landing height, speed, state,
 *             value and bounce count
 *     0x4d14  0x20 sparkles, 0x18 each: position, velocity, life
 *     0x5014  24 score popups, 0x10 each
 *     0x519c  the caption animation: delay, frame time, state, sprite
 *     0x51a8  the bouncing record: position, velocity, spin, countdown,
 *             bounces left and stage
 *     0x51c8  the scene phase (5 is the last), advanced only by
 *             func_ov006_020dc5c4
 */

#include "types.h"
#include "decl_common.h"
#include "Sound.h"

#pragma defer_codegen off

// @symbol func_ov006_020dbf7c
extern "C" {
/* Steps one +0x51a8 record: integrate, spin it a little faster in whichever
 * direction it is travelling, and once it has fallen past the height for its
 * stage, bounce it back up with a random horizontal speed.  bouncesLeft counts
 * down within a stage; after stage 6 the record just stops being drawn once
 * it is off the bottom. */
extern int RandomIntInternal(int *seed);

extern int data_0209d4b8;
extern int data_ov006_0212e370[];
extern int data_ov006_0212e388[];
extern int data_ov006_0212e344[];

typedef struct {
    int x;
    int y;
    int vx;
    int vy;
    unsigned short spin;
    unsigned short field_12;
    unsigned char field_14;
    unsigned char field_15;
    unsigned char field_16;
    unsigned char field_17;
    unsigned char bouncesLeft;
    unsigned char stage;
    unsigned char field_1a;
    unsigned char field_1b;
} Bouncer;

struct BouncerView {
    unsigned char pad[0x51a8];
    Bouncer bouncers[1];
};

void func_ov006_020dbf7c(struct BouncerView *scene, int i)
{
    int height;
    int stageIndex;

    scene->bouncers[i].x += scene->bouncers[i].vx;
    scene->bouncers[i].y += scene->bouncers[i].vy;
    scene->bouncers[i].vy += 0x400;
    if (scene->bouncers[i].vx > 0) {
        scene->bouncers[i].spin += 0x400;
    } else {
        scene->bouncers[i].spin -= 0x400;
    }

    height = scene->bouncers[i].y >> 12;
    stageIndex = scene->bouncers[i].stage;

    if (stageIndex >= 6) {
        if (height >= 0xd0) {
            scene->bouncers[i].field_15 = 0;
            scene->bouncers[i].field_14 = 0;
        }
        return;
    }

    if (scene->bouncers[i].bouncesLeft != 0) {
        unsigned int roll;
        if (height < (data_ov006_0212e370[stageIndex] - data_ov006_0212e388[stageIndex])) {
            return;
        }
        roll = (unsigned int)RandomIntInternal(&data_0209d4b8);
        scene->bouncers[i].vx = data_ov006_0212e344[((roll >> 16 & 0x7fff) << 1) >> 15];
        scene->bouncers[i].vy = -0x3000;
        scene->bouncers[i].bouncesLeft--;
        func_02012718(0xef, scene->bouncers[i].x);
        if (scene->bouncers[i].bouncesLeft == 0) {
            scene->bouncers[i].stage++;
        }
    } else {
        scene->bouncers[i].bouncesLeft = 1;
    }
}
}

// @symbol func_ov006_020dc154
extern "C" {
/* Runs the countdown at +0x51ba on one +0x51a8 record; when it expires the
 * record goes to state 3 and the scene is told to re-scan. */
void func_ov006_020dc154(char *raw, int index)
{
    extern void func_ov006_020ddeb0(char *rec);

    unsigned short *timer = (unsigned short *) ((raw + 0x51ba) + (index * 0x1c));
    if ((*timer) != 0) {
        *timer = (*timer) - 1;
        return;
    }
    {
        /* Only the first store goes through rec; spelling the other three out from
         * raw again is what the original does, and folding them into rec costs
         * eight words. */
        char *rec = (raw + (index * 0x1c)) + 0x5000;
        *((unsigned char *) (rec + 0x1bf)) = 3;
        *((int *) (((raw + (index * 0x1c)) + 0x5000) + 0x1b4)) = 0;
        *((unsigned char *) (((raw + (index * 0x1c)) + 0x5000) + 0x1c0)) = 1;
        *((unsigned char *) (((raw + (index * 0x1c)) + 0x5000) + 0x1c1)) = 0;
    }
    func_ov006_020ddeb0(raw);
}
}

// @symbol func_ov006_020dc1c4
extern "C" {
/* The falling half of the same record: apply gravity and spin until it reaches
 * the floor at -0xf0, then park it there, put it in state 2 and start the 0x30
 * frame countdown func_ov006_020dc154 runs down. */
void func_ov006_020dc1c4(void *self, int index)
{
    extern void func_ov006_020ddeb0(void *self);

    unsigned char *raw = (unsigned char *)self;
    int off = index * 0x1c;

    *(int *)(raw + 0x51ac + off) = *(int *)(raw + 0x51ac + off) + *(int *)(raw + 0x51b4 + off);
    *(int *)(raw + 0x51b4 + off) = *(int *)(raw + 0x51b4 + off) - 0x100;
    *(unsigned short *)(raw + 0x51b8 + off) = *(unsigned short *)(raw + 0x51b8 + off) + 0x400;

    if ((*(int *)(raw + 0x51ac + off) >> 0xc) > -0xf0)
        return;

    *(int *)(raw + 0x51ac + off) = -0xf0000;
    *(unsigned char *)(raw + 0x51bf + off) = 2;
    *(unsigned short *)(raw + 0x51ba + off) = 0x30;
    func_ov006_020ddeb0(raw);
}
}

// @symbol func_ov006_020dc26c
extern "C" {
/* Kicks record 0 into state 1 and gives it a little upward hop. */
void func_ov006_020dc26c(char *raw)
{
    *(char *)(raw + 0x51bf) = 1;
    *(char *)(raw + 0x51be) = 1;
    *(int *)(raw + 0x51b0) = 0;
    *(int *)(raw + 0x51b4) = -0x4000;
}
}

// @symbol func_ov006_020dc294
extern "C" {
/* Deliberately empty: the first entry of the scene's callback table at
 * 0x0213be44 points here, so state 0 does nothing. */
void func_ov006_020dc294(void)
{
}
}

// @symbol func_ov006_020dc298
extern "C" {
struct C_c298;
typedef void (C_c298::*PMF_c298)(int);
struct Entry_c298 { PMF_c298 pmf[1]; };
extern Entry_c298 data_ov006_021417c8[];
struct C_c298 {
    char pad[0x51bc];
    unsigned char guard;
    char pad2[2];
    unsigned char idx;
};

/* Per-frame step for the +0x51a8 records: dispatch on the state byte at +0x51bf
 * through the pointer-to-member table at 0x021417c8, but only while the enable
 * flag at +0x51bc is set. */
void func_ov006_020dc298(C_c298 *scene)
{
    if (scene->guard == 0)
        return;

    int state = scene->idx;
    (scene->*data_ov006_021417c8[state].pmf[0])(0);
}
}

// @symbol func_ov006_020dc2f8
extern "C" {
/* Resets the +0x51a8 records to their start-of-round position and enables them. */
void func_ov006_020dc2f8(char *raw)
{
    *(char *)(raw + 0x51bc) = 1;
    *(char *)(raw + 0x51bd) = 1;
    *(char *)(raw + 0x51be) = 0;
    *(char *)(raw + 0x51bf) = 0;
    *(short *)(raw + 0x51b8) = 0;
    *(int *)(raw + 0x51a8) = 0x8e000;
    *(int *)(raw + 0x51ac) = -0x43000;
}
}

// @symbol func_ov006_020dc334
extern "C" {
/* The other half of func_ov006_020dc2f8: clears both enable flags. */
void func_ov006_020dc334(char *raw)
{
    *(char *)(raw + 0x51bc) = 0;
    *(char *)(raw + 0x51bd) = 0;
}
}

// @symbol func_ov006_020dc348
extern "C" {
/* Puts the +0x519c animation into state 4 from frame 0 and latches the flag at
 * +0x51de, which func_ov006_020dc370 then reads as "leave me alone". */
void func_ov006_020dc348(char *raw)
{
    *(char *)(raw + 0x51a1) = 4;
    *(char *)(raw + 0x51a4) = 0;
    *(short *)(raw + 0x519e) = 0;
    *(char *)(raw + 0x51de) = 1;
}
}

// @symbol func_ov006_020dc370
extern "C" {
/* Starts, or retriggers, the 0x3c frame state-2 animation.  Retriggering only
 * refreshes the delay; the frame counter is left alone. */
void func_ov006_020dc370(char *raw)
{
    if (*(unsigned char *)(raw + 0x51de) != 0)
        return;

    if (*(unsigned char *)(raw + 0x51a1) == 2) {
        *(short *)(raw + 0x519c) = 0x3c;
        return;
    }

    *(short *)(raw + 0x519c) = 0x3c;
    *(char *)(raw + 0x51a1) = 2;
    *(char *)(raw + 0x51a4) = 0;
    *(short *)(raw + 0x519e) = 0;
}
}

// @symbol func_ov006_020dc3bc
extern "C" {
extern int data_ov006_0213a9fc[];

/* Draws the +0x5194 sprite, if it is visible, at its own fixed-point position.
 * The sprite index at +0x51a3 selects a cell out of data_ov006_0213a9fc. */
void func_ov006_020dc3bc(char *raw)
{
    extern int RenderOamMainScreen(int a, int b, int raw, int d, int e);

    if (*(unsigned char *)(raw + 0x51a2) == 0)
        return;

    int x = *(int *)(raw + 0x5194);
    int y = *(int *)(raw + 0x5198);
    RenderOamMainScreen(data_ov006_0213a9fc[*(unsigned char *)(raw + 0x51a3)],
                        x >> 0xc, y >> 0xc, -1, -1);
}
}

// @symbol func_ov006_020dc414
extern "C" {
extern unsigned char data_ov006_0212e324[];
extern unsigned char data_ov006_0212e31c[];

/* Eight-frame looping animation: hold each frame for its own duration out of
 * data_ov006_0212e324, then step the sprite index out of data_ov006_0212e31c.
 * Frame 0 is the intro, so the loop comes back round to 1 rather than 0. */
void func_ov006_020dc414(char *raw, int index)
{
    unsigned short *frameTime = (unsigned short *)(raw + 0x519e + index * 0x14);
    unsigned char *frame = (unsigned char *)(raw + 0x51a4 + index * 0x14);

    *frameTime = *frameTime + 1;
    if (*frameTime < data_ov006_0212e324[*frame])
        return;

    *frameTime = 0;
    *frame = *frame + 1;
    if (*frame >= 8) {
        *frame = 1;
    } else {
        *(unsigned char *)(raw + index * 0x14 + 0x51a3) = data_ov006_0212e31c[*frame];
    }
}
}

// @symbol func_ov006_020dc4b0
extern "C" {
/* Back to sprite cell 0. */
void func_ov006_020dc4b0(char *raw, int index)
{
    *(char *)(raw + index * 0x14 + 0x51a3) = 0;
}
}

// @symbol func_ov006_020dc4c8
extern "C" {
extern unsigned char data_ov006_0212e308[];
extern unsigned char data_ov006_0212e30c[];

void func_ov006_020dc4c8(char *raw, int index)
{
    int off = index * 0x14;
    unsigned short frameTime = *((unsigned short *) ((raw + 0x519e) + off));
  *((unsigned short *) ((raw + 0x519e) + off)) = frameTime + 1;
    unsigned char frame = *((unsigned char *) ((raw + 0x51a4) + off));
    if ((*((unsigned short *) ((raw + 0x519e) + off))) >= data_ov006_0212e308[frame]) {
    *((unsigned short *) ((raw + 0x519e) + off)) = 0;
    *((unsigned char *) ((raw + 0x51a4) + off)) = (*((unsigned char *) ((raw + 0x51a4) + off))) + 1;
    *((unsigned char *) ((raw + 0x51a4) + off)) = (*((unsigned char *) ((raw + 0x51a4) + off))) & 1;
    }
  *((unsigned char *) ((raw + off) + 0x51a3)) = data_ov006_0212e30c[*((unsigned char *) ((raw + 0x51a4) + off))];
    if ((*((int *) (raw + 0x51c8))) == 5) {
        return;
    }
    unsigned short delay = *((unsigned short *) ((raw + 0x519c) + off));
    if (delay != 0) {
    *((unsigned short *) ((raw + 0x519c) + off)) = delay - 1;
        return;
    }
  *((raw + off) + 0x51a1) = 3;
  *((unsigned char *) ((raw + 0x51a4) + off)) = 0;
  *((unsigned short *) ((raw + 0x519e) + off)) = 0;
  *((unsigned short *) ((raw + 0x519c) + off)) = 0;
}
}

// @symbol func_ov006_020dc5c4
extern "C" {
extern void func_ov006_020dc26c(char *c);
extern u16 data_ov006_0212e33c[];
extern u8 data_ov006_0212e310[];

/* The count-in animation.  Steps through data_ov006_0212e33c's per-frame
 * durations; frame 2 hops record 0 and plays the "ready" sound, and after frame 3
 * the slot drops into state 2 with a 0xb4 frame delay and the scene phase at
 * +0x51c8 advances.  This is the only place that phase word is incremented. */
void func_ov006_020dc5c4(char *raw, int index)
{
    int off = index * 0x14;
    u16 *frameTime = (u16 *)(raw + 0x519e + off);
    u8  *frame = (u8 *)(raw + 0x51a4 + off);
    *frameTime = (u16)(*frameTime + 1);
    if (*frameTime < data_ov006_0212e33c[*frame])
        return;
    *frameTime = 0;
    *frame = (u8)(*frame + 1);
    if (*frame == 2) {
        func_ov006_020dc26c(raw);
        Sound::PlayBank2_2D(0xf0);
    }
    if (*frame >= 4) {
        *(u16 *)(raw + 0x519c + off) = 0xb4;
        *(u8 *)(raw + 0x51a1 + off) = 2;
        {
            int *phase = (int *)(raw + 0x51c8);
            *frame = 0;
            *frameTime = 0;
            *phase = *phase + 1;
        }
    } else {
        *(u8 *)(raw + 0x51a3 + off) = data_ov006_0212e310[*frame];
    }
}
}

// @symbol func_ov006_020dc6d0
extern "C" {
/* Runs the state-2 delay down, chiming 0x18 frames before it expires, and then
 * hands the slot back to state 1 at frame 0.
 *
 * The (unsigned long long) on the last line is load-bearing, not litter: taking
 * it out costs five words. */
void func_ov006_020dc6d0(int raw, int index)
{
    unsigned short *timer = (unsigned short *) ((((char *) raw) + 0x519c) + (index * 0x14));
    if ((*timer) != 0) {
        *timer = (*timer) - 1;
        if ((*timer) == 0x18) {
            Sound::PlayBank2_2D(0xf1);
        }
        return;
    }
    *((unsigned char *) (((((char *) raw) + (index * 0x14)) + 0x5000) + 0x1a1)) = 1;
    *((unsigned short *) (((((char *) ((unsigned long long) raw)) + (index * 0x14)) + 0x5000) + 0x19e)) = 0;
}
}

// @symbol func_ov006_020dc754
extern "C" {
struct C_c754;
typedef void (C_c754::*PMF_c754)(int);
struct Entry_c754 { PMF_c754 pmf[1]; };
extern Entry_c754 data_ov006_021417e8[];
struct C_c754 {
    char pad[0x51a0];
    unsigned char guard;
    unsigned char idx;
};

/* Same shape as func_ov006_020dc298, one record set over: dispatch the +0x519c
 * slot's state byte through the table at 0x021417e8 while its flag is set. */
void func_ov006_020dc754(C_c754 *scene)
{
    if (scene->guard == 0)
        return;

    int state = scene->idx;
    (scene->*data_ov006_021417e8[state].pmf[0])(0);
}
}

// @symbol func_ov006_020dc7b4
extern "C" {
/* Start of round: park the +0x519c slot at the middle of the screen, visible, in
 * state 0 on sprite cell 3, with 0x28 frames on the clock. */
void func_ov006_020dc7b4(char *raw)
{
    *(char *)(raw + 0x51a0) = 1;
    *(char *)(raw + 0x51a1) = 0;
    *(char *)(raw + 0x51a2) = 1;
    *(short *)(raw + 0x519c) = 0x28;
    *(short *)(raw + 0x519e) = 0;
    *(char *)(raw + 0x51a3) = 3;
    *(char *)(raw + 0x51a4) = 0;
    *(int *)(raw + 0x5194) = 0x70000;
    *(int *)(raw + 0x5198) = 0x98000;
}
}

// @symbol func_ov006_020dc7fc
extern "C" {
/* End of round: stop stepping the slot but leave it on screen. */
void func_ov006_020dc7fc(char *raw)
{
    *(char *)(raw + 0x51a0) = 0;
    *(char *)(raw + 0x51a2) = 1;
}
}

// @symbol func_ov006_020dc814
extern "C" {
extern void func_ov004_020b0d8c(void *c, int arg1, int arg2);

/* In the last phase only, and only over the final 0x80 frames, draw the
 * countdown overlay.  Nothing at all once it reaches zero. */
void func_ov006_020dc814(char *raw)
{
    if (*(int *)(raw + 0x51c8) != 5)
        return;

    int left = *(int *)(raw + 0x51cc);
    if (left > 0x80)
        return;
    if (left == 0)
        return;

    func_ov004_020b0d8c(raw, 0xe0, 0xa0);
}
}

// @symbol func_ov006_020dc870
extern "C" {
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);

typedef struct {
    int x;
    int y;
    unsigned char p0[2];
    unsigned short num;
    unsigned char p1;
    unsigned char flag;
    unsigned char p2[2];
} CoinPopup;

typedef struct {
    unsigned char _pad[0x5014];
    CoinPopup popups[24];
} CoinPopupView;

/* Draws all 24 live +0x5014 popups.  Once the scene is in its last phase they
 * are suppressed as soon as the countdown at +0x51cc hits zero. */
void func_ov006_020dc870(CoinPopupView *view)
{
    int i;
    if (*(int *)((char *)view + 0x51c8) == 5) {
        if (*(int *)((char *)view + 0x51cc) == 0)
            return;
    }
    for (i = 0; i < 0x18; i++) {
        if (view->popups[i].flag != 0) {
            func_ov004_020b2444(view->popups[i].x >> 12, view->popups[i].y >> 12, view->popups[i].num, 0, -1, 0, 0);
        }
    }
}
}

// @symbol func_ov006_020dc900
extern "C" {
/* Ages the 24 +0x5014 popups a frame each and marks the expired ones done.
 *
 * Both cast towers here are load-bearing.  The (long long) on the index and the
 * (int) round-trip on the two +0x501c accesses each cost a word if removed --
 * the (int) is what stops mwccarm reusing the address it just computed for the
 * read-modify-write. */
void func_ov006_020dc900(char *raw)
{
    int i;
    for (i = 0; i < 0x18; i++) {
        char *popup = raw + (int)((long long)i) * 0x10;
        if (*(u8 *)(popup + 0x5020) != 0 && *(u16 *)(popup + 0x501c) != 0) {
            *(u16 *)((int)(popup + 0x501c)) -= 1;
            if (*(s16 *)((int)(popup + 0x501c)) <= 0)
                *(u8 *)(popup + 0x5021) = 1;
        }
    }
}
}

// @symbol func_ov006_020dc960
extern "C" {
/* Spawns popup i over coin i: same position, the coin's value, 0x18 frames.
 * The two strides differ -- 0x18 for the coin, 0x10 for the popup. */
void func_ov006_020dc960(char *raw, int index)
{
    char *coin = raw + index * 0x18;
    char *popup = raw + index * 0x10;
    *(int *)(popup + 0x5014) = *(int *)(coin + 0x4ac0);
    *(int *)(popup + 0x5018) = *(int *)(coin + 0x4ac4);
    *(short *)(popup + 0x501e) = *(unsigned char *)(coin + 0x4ad3);
    *(unsigned char *)(popup + 0x5020) = 1;
    *(short *)(popup + 0x501c) = 0x18;
}
}

// @symbol func_ov006_020dc99c
extern "C" {
extern void *data_ov006_02136e24[];
struct E_c99c { char pad[0x18]; };

/* Draws whichever of the 0x20 +0x4d14 sparkles are visible, each with its own
 * sprite out of data_ov006_02136e24. */
void func_ov006_020dc99c(char *raw)
{
    extern void func_ov004_020b0380(void *fn, int x, int y, int d);

    int i;
    struct E_c99c *rows = (struct E_c99c *)raw;
    for (i = 0; i < 0x20; i++) {
        char *sparkle = (char *)&rows[i];
        if (*(unsigned char *)(sparkle + 0x4d29) != 0) {
            int sprite = *(unsigned char *)(sparkle + 0x4d2a);
            int x = *(int *)(sparkle + 0x4d14);
            int y = *(int *)(sparkle + 0x4d18);
            func_ov004_020b0380(data_ov006_02136e24[sprite], x >> 0xc, y >> 0xc, 0);
        }
    }
}
}

// @symbol func_ov006_020dca04
extern "C" {
/* Steps the 0x20 +0x4d14 sparkles: gravity, and horizontal drag that pulls any
 * speed above 0x300 back towards it.  The early return when one expires is the
 * original's, not a transcription slip -- only one sparkle is retired a frame. */
void func_ov006_020dca04(char *raw)
{
    int i;
    char *sparkle = raw;
    for (i = 0; i < 0x20; i++, sparkle += 0x18) {
        if (*(u8 *)(sparkle + 0x4d28) != 0) {
            if (*(u16 *)(sparkle + 0x4d24) != 0) {
                *(u16 *)(sparkle + 0x4d24) -= 1;
            } else {
                *(u8 *)(raw + i * 0x18 + 0x4d28) = 0;
                *(u8 *)(raw + i * 0x18 + 0x4d29) = 0;
                return;
            }
            *(int *)(sparkle + 0x4d14) += *(int *)(sparkle + 0x4d1c);
            *(int *)(sparkle + 0x4d18) += *(int *)(sparkle + 0x4d20);
            *(int *)(sparkle + 0x4d20) += 0x180;
            if (*(int *)(sparkle + 0x4d1c) > 0x300)
                *(int *)(sparkle + 0x4d1c) -= 0x180;
            else if (*(int *)(sparkle + 0x4d1c) < -0x300)
                *(int *)(sparkle + 0x4d1c) += 0x180;
        }
    }
}
}

// @symbol func_ov006_020dcb1c
extern "C" {
extern int data_ov006_0212e430[];
extern u16 data_ov006_0212e334[];
extern s16 data_02082214[];

/* Bursts four sparkles out of coin a1, one per fixed direction in
 * data_ov006_0212e334.  The offsets come in x/z pairs out of
 * data_ov006_0212e430, the speeds off the shared sine table at 0x02082214. */
void func_ov006_020dcb1c(char *raw, int coinIndex)
{
    int i, j, k;
    char *coin = raw + coinIndex * 0x18;
    int *coinX = (int *)(coin + 0x4ac0);
    int *coinY = (int *)(coin + 0x4ac4);
    for (i = 0, j = 0, k = 1; i < 4; i++, raw += 0x18, j += 2, k += 2) {
        if (*(u8 *)(raw + 0x4d28) == 0) {
            *(u8 *)(raw + 0x4d28) = 1;
            *(u8 *)(raw + 0x4d29) = 1;
            *(int *)(raw + 0x4d14) = *coinX + (data_ov006_0212e430[j] << 12);
            *(int *)(raw + 0x4d18) = *coinY + (data_ov006_0212e430[k] << 12);
            {
                u16 ang = data_ov006_0212e334[i];
                int sine = (ang >> 4) * 2;
                *(int *)(raw + 0x4d1c) = (int)(((s64)data_02082214[sine + 1] * 0x2000 + 0x800) >> 12);
                *(int *)(raw + 0x4d20) = (int)(((s64)data_02082214[sine] * 0x2000 + 0x800) >> 12);
            }
            *(u16 *)(raw + 0x4d24) = 0x18;
        }
    }
}
}

// @symbol func_ov006_020dcc48
extern "C" {
extern int data_ov006_021341ec;
extern int data_ov006_0212e364[];

/* Tiles the backdrop: three rows of sixteen, 0x20 apart, all the same sprite. */
void func_ov006_020dcc48(void)
{
    extern void func_ov004_020b0380(int a, int b, int c, int d);

    int i;
    int x, y, j;
    for (i = 0; i < 3; i++) {
        y = data_ov006_0212e364[i];
        x = 0x10;
        for (j = 0; j < 0x10; j++) {
            func_ov004_020b0380(data_ov006_021341ec, x, y, 0);
            x += 0x20;
        }
    }
}
}

// @symbol func_ov006_020dccb8
extern "C" {
extern int *data_ov006_0213bf34[];
int GetGameLanguage(void);
void func_ov004_020b1de8(int r0, int r1, int r2, int r3);
void DrawOamSprite(void *arg0, void *arg1, int arg2, void *arg3);
void func_ov004_020b2444(int a0, int a1, int a2, int a3, int a4, int a5, int a6);

/* The top line of the score display, once the scene is past its opening
 * phase: a caption from the current language's table, a label, and the count
 * at +0x51d4. GetGameLanguage is called twice on purpose: hoisting the
 * second call costs eighteen words. */
void func_ov006_020dccb8(char *raw)
{
    void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);

    int lang;
    if (*(int *)(raw + 0x51c8) < 2)
        return;

    lang = GetGameLanguage();
    RenderOamMainScreen(data_ov006_0213bf34[lang][8], 0x80, 0x18, -1, -1);
    func_ov004_020b1de8(0x68, 0x28, 1, -1);
    lang = GetGameLanguage();
    DrawOamSprite((void *)data_ov006_0213bf34[lang][1], (void *)0x7a, 0x28, (void *)0);
    func_ov004_020b2444(0x8c, 0x28, *(int *)(raw + 0x51d4), 1, -1, 2, 0);
}
}

// @symbol func_ov006_020dcd74
extern "C" {
extern int *data_ov006_0213bf34[];
extern int *data_ov004_020beb68;
int GetGameLanguage(void);
void func_ov004_020b1de8(int r0, int r1, int r2, int r3);
void DrawOamSprite(void *arg0, void *arg1, int arg2, void *arg3);
void func_ov004_020b2444(int a0, int a1, int a2, int a3, int a4, int a5, int a6);

/* The second line of the score display, laid out exactly like the first, except that the
 * number is the other player's and is read out of ov004's record -- which may
 * not exist yet, in which case it shows zero. */
void func_ov006_020dcd74(char *raw)
{
    void RenderOamMainScreen(int a0, int a1, int a2, int a3, int a4);

    int lang;
    int count;
    if (*(int *)(raw + 0x51c8) < 2)
        return;

    lang = GetGameLanguage();
    RenderOamMainScreen(data_ov006_0213bf34[lang][4], 0x80, 0x50, -1, -1);
    func_ov004_020b1de8(0x6c, 0x60, 1, -1);
    lang = GetGameLanguage();
    DrawOamSprite((void *)data_ov006_0213bf34[lang][1], (void *)0x7e, 0x60, (void *)0);
    if (data_ov004_020beb68 != 0)
        count = *(int *)((char *)data_ov004_020beb68 + 0xac);
    else
        count = 0;
    func_ov004_020b2444(0x90, 0x60, count, 1, -1, 2, 0);
}
}

// @symbol func_ov006_020dce3c
extern "C" {
/* Walks the displayed total at +0x4d0a up towards the real one at +0x4d08, one
 * step every eight frames, so the score counts up rather than jumping. */
void func_ov006_020dce3c(char *raw)
{
    if (*(unsigned char *)(raw + 0x4d13) == 0)
        return;
    if (*(unsigned short *)(raw + 0x4d08) == *(unsigned short *)(raw + 0x4d0a))
        return;
    {
        unsigned short *tick = (unsigned short *)(raw + 0x4d0c);
        *tick = *tick + 1;
    }
    if (*(unsigned short *)(raw + 0x4d0c) < 8)
        return;
    *(unsigned short *)(raw + 0x4d0c) = 0;
    {
        unsigned short *shown = (unsigned short *)(raw + 0x4d0a);
        *shown = *shown + 1;
    }
}
}

// @symbol func_ov006_020dcea8
extern "C" {
struct Coin {
    int x;
    int y;
    char pad8[8];
    unsigned char state;
    unsigned char b11;
    unsigned char visible;
    unsigned char value;
    unsigned char b14;
    unsigned char b15;
    unsigned char bounces;
    unsigned char b17;
};

struct CoinView {
    char pad0[0x4ac0];
    struct Coin coins[24];
    char pad1[0x4c8];
    int phase;
    int countdown;
};

extern void func_ov004_020afdd0(void* a0, int a1, int a2, int a3, int a4);
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void* data_ov006_02133f10[];

/* Draws the 24 coins.  Before the last phase a coin is drawn whenever visible is
 * set; in the last phase only the ones that were collected are, with their value
 * printed over them.  state == 2 picks the second sprite and the highlight. */
void func_ov006_020dcea8(struct CoinView *view)
{
    int i;
    int v1, v2;
    if (view->phase == 5 && view->countdown == 0)
        return;

    v1 = 0;
    v2 = 0;
    for (i = 0; i < 24; i++) {
        int st = view->coins[i].state;
        int sprite = (st == 2) ? 1 : v1;
        int x = view->coins[i].x >> 12;
        int y = view->coins[i].y >> 12;
        int highlight = (view->phase == 5) ? 1 : v2;
        if (st == 2) highlight = 1;
        if (view->phase != 5) {
            if (view->coins[i].visible != 0) {
                func_ov004_020afdd0(data_ov006_02133f10[sprite], x, y, -1, highlight);
            }
        } else {
            if (sprite != 0 && view->coins[i].value != 0) {
                func_ov004_020afdd0(data_ov006_02133f10[sprite], x, y, -1, highlight);
            }
            if (view->coins[i].b15 == 0 && view->coins[i].value != 0) {
                func_ov004_020b2444(x, y, view->coins[i].value, 0, -1, 0, 0);
            }
        }
    }
}
}

// @symbol func_ov006_020dcffc
extern "C" {
/* Another table no-op, like func_ov006_020dc294. */
void func_ov006_020dcffc(void)
{
}
}

// @symbol func_ov006_020dd000
extern "C" {
extern void func_ov006_020dc370(char *c);
extern void func_ov006_020dc960(char *c, int i);
extern void func_ov006_020dd4b0(char *c, int i);

/* One hop of a bouncing coin: fall, land on the floor recorded at +0x4ac8, count
 * the bounce and start the caption animation.  After the last bounce the coin
 * settles into state 2 and leaves a popup behind; otherwise it goes back up. */
void func_ov006_020dd000(char *raw, int index)
{
    int off = index * 0x18;
    *(s32 *)(raw + 0x4ac4 + off) = *(s32 *)(raw + 0x4ac4 + off) + *(s32 *)(raw + 0x4acc + off);
    *(s32 *)(raw + 0x4acc + off) = *(s32 *)(raw + 0x4acc + off) + 0x400;
    if (*(s32 *)(raw + 0x4ac4 + off) < *(s32 *)(raw + 0x4ac8 + off)) {
        return;
    }
    *(s32 *)(raw + 0x4ac4 + off) = *(s32 *)(raw + 0x4ac8 + off);
    *(u8 *)(raw + 0x4ad6 + off) = *(u8 *)(raw + 0x4ad6 + off) + 1;
    func_ov006_020dc370(raw);
    if (*(u8 *)(raw + 0x4ad3 + off) == *(u8 *)(raw + 0x4ad6 + off)) {
        *(u8 *)(raw + 0x4ad0 + off) = 2;
        func_ov006_020dc960(raw, index);
        return;
    }
    *(s32 *)(raw + 0x4acc + off) = -0x3000;
    func_ov006_020dd4b0(raw, index);
}
}
