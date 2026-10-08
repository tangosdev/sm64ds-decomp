//cpp
/* Two-player curling -- the upper half of dScMgCurling2_c, the part of the
 * TU the linker placed above the sourceless hole at func_ov006_020e5450.
 * Behavior state machine: BeginRound seeds a round into Play, Play ticks
 * the drag table and every live stone and hands off to NextThrow once all
 * eleven rest, NextThrow either deals the next stone or goes to EndRound,
 * and Idle is the terminal state. The stones themselves run a second
 * machine -- StoneWait (armed, thrown when the dragged stylus reaches
 * them), StoneSlide, StoneRest and StoneSteer -- dispatched through the
 * same pointer-to-member table shape as the lower TU.
 *
 * Leftover: func_ov006_020e5450 (the stone-collision step StoneRest
 *   veneers to, and StoneSlide calls directly) is unmatched; a banked
 *   draft at 30 divergent words of 344 lives in src/unnamed/ov006/func_ov006_020e5450.cpp.
 *   StoneRest and StoneSlide keep the extern "C" call so the bytes still
 *   veneer to it.
 * Leftover: StoneSpin, StoneSlide, NextStone, SeedStones, Play and
 *   ResetGame keep the ROM's raw `this + 0x46xx/0x55xx` addressing (and
 *   SeedStones its `struct E` + 0x4000 split base). Folding them through
 *   the named mStone/mMark/field members loses the base rematerialization,
 *   same as SpawnValue and the two mark functions in the lower TU.
 */

/* Required: ROM order. */
#pragma defer_codegen off

#include "types.h"
#include "dScMgCurling2_c.h"
#include "PlayerInput.h"

/* The receiver for the pointer-to-member tables. It must stay incomplete:
 * mwccarm picks the pointer-to-member layout from whether the class is
 * complete. */
struct C;
typedef void (C::*PMF0)();
typedef void (C::*PMF1)(int);

/* Everything this file calls or reads that no included header declares.
 * Keep it above the first function. */
extern "C" {

extern int  func_ov004_020adbe0(void);
extern int  func_ov004_020adc1c(void);
extern void func_ov004_020adb1c(int self);
extern void *func_ov004_020adc74(const char *p);
extern void func_ov004_020b04d0(int a);
extern void func_ov004_020b0a54(s32 state);
extern int  func_ov004_020b19f0(void *self);
extern void func_02012718(int id, int v);
extern int  func_020126e8(int a);
extern int  func_02012468(int a, int b, int c, int d, int e, int f, int g, s16 h);
extern void func_020126ac(int a0, int a1, int a2, int a3, int s0);
extern void func_ov006_020e5450(dScMgCurling2_c *self, int idx);
extern int  RandomIntInternal(int *seed);
extern void SetBg2Offset(int a, int b);
extern void SetBg0Offset(int a, int b);
extern void DecompressLZ16(const void *src, void *dst);
extern int  LoadFile(int handle);
extern void Deallocate(void *p);
extern void Ov004_Deallocate(void *p);
extern void MultiStore16(u16 val, char *dst, int nbytes);
extern void func_02056554(const void *src, int offset, int count);
extern void func_02056314(void *a, u32 b, u32 c);
extern void func_020563d4(const void *a, u32 b, u32 c);
extern void func_02056374(const void *a, u32 b, u32 c);
extern int  func_02054d88(void);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern void *_ZN2G212GetBG0ScrPtrEv(void);
extern unsigned _ZN3G2S13GetBG2CharPtrEv(void);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile u16 *p, u16 a, u16 b, u16 c, u32 d);

extern s16  data_02082214[];
extern int  data_0209d4b8;
extern u8   data_0209d45c;
extern u8   data_0209d454;
extern int  data_ov006_0212e504[];
extern int  data_ov006_0212e51c[];
extern int  data_ov006_0212e534[];
extern int  data_ov006_0212e554[];
extern char data_ov006_0213c5a0;

/* The three pointer-to-member tables this side of the hole dispatches
 * through. __sinit_ov006_02130758 fills them from the 8-byte {code
 * pointer, zero adjustment} records in ov006 .data. */
extern PMF0 data_ov006_02141978[];
extern PMF1 data_ov006_021419d8[];
extern PMF0 data_ov006_02141a18[];

}  /* extern "C" */

namespace Sound { void PlayBank2_2D(unsigned int id); }


// @symbol _ZN15dScMgCurling2_c9StoneSpinEi
/* Spin-rate update for the stone sprite: -sin(heading) * speed, scaled.
 * The offsets stay raw this + i * 0x30 + 0x46xx, like SpawnValue's
 * 0x4000 base in the lower TU: folding them through &mStone[i] changes
 * the base rematerialization. */
void dScMgCurling2_c::StoneSpin(int i)
{
    char *o = (char *)this + i * 0x30;
    int h = *(unsigned short *)(o + 0x4686);
    int idx = (((h >> 4) << 1) + 1) << 1;
    int s = *(short *)((char *)data_02082214 + idx);
    int v = *(int *)(o + 0x4668);
    long long m = (long long)s * v;
    int hi = (int)(((unsigned long long)(m + 0x800)) >> 12);
    *(short *)(o + 0x4682) = (short)((-hi) >> 2);
}


// @symbol _ZN15dScMgCurling2_c10StoneSteerEi
/* Stone state 3: while the touch is down, the stylus feeds velX/velY out
 * of the input-record deltas (x clamped to the rink); once it lifts, the
 * stone is armed again and, when the drag point is close, snaps above it. */
void dScMgCurling2_c::StoneSteer(int idx)
{
    u8 i = gActivePlayerSlot;
    int off = i * 4;
    if (gTouchHeld[i * 4] != 0) {
        int bp = mStone[idx].velX;
        mStone[idx].x = bp + (gTouchX[off] << 12);
        int t = mStone[idx].x >> 12;
        if (t < 0xe) mStone[idx].x = 0xe000;
        if (t > 0xf2) mStone[idx].x = 0xf2000;
        int av = mStone[idx].x >> 12;
        int bv = av - gTouchX[i * 4];
        int cv = mStone[idx].y >> 12;
        int dv = cv - gTouchY[i * 4];
        mStone[idx].velX = bv << 12;
        mStone[idx].velY = dv << 12;
    } else {
        mStone[idx].state = 0;
        int dx = (unk_5584 - mStone[idx].x) >> 12;
        int cy = mStone[idx].y;
        int dy = (unk_5588 - cy) >> 12;
        if (dx < -0x2e) return;
        if (dx > 0x2e) return;
        if (dy < -0x14) return;
        if (dy > 0x14) return;
        unk_5588 = cy + 0x15000;
    }
}


// @symbol _ZN15dScMgCurling2_c9StoneRestEi
/* Stone state 2: the resting handler veneers to the sourceless collision
 * step. See the file-top leftover for func_ov006_020e5450. */
void dScMgCurling2_c::StoneRest(int i)
{
    func_ov006_020e5450(this, i);
}


// @symbol _ZN15dScMgCurling2_c10StoneSlideEi
/* Stone state 1: integrate by speed at the heading, bounce off the rink
 * walls (the bounce flips the heading and plays 0x1d4), decay speed to a
 * stop, run the collision step and the spin update, then feed the rolling
 * sound from speed and position. */
void dScMgCurling2_c::StoneSlide(int idx)
{
    char *c = (char *)this;
    int m;
    int zi;
    u16 *pang;
    int *p668;
    int *p660;
    int *p664;
    int x;
    int z;
    int xi;
    int v;
    int p;
    int w;
    int *pd;
    int sn;
    int cs;

    m = idx * 0x30;

    pang = (u16 *)(c + 0x4686 + m);
    p668 = (int *)(c + 0x4668 + m);
    p660 = (int *)(c + 0x4660 + m);
    p664 = (int *)(c + 0x4664 + m);

    sn = data_02082214[((*pang) >> 4) * 2 + 1];
    *p660 += (int)(((long long)sn * *p668 + 0x800) >> 12);
    cs = data_02082214[((*pang) >> 4) * 2];
    *p664 += (int)(((long long)cs * *p668 + 0x800) >> 12);
    *(u16 *)(c + 0x4684 + m) += *(u16 *)(c + m + 0x4682);

    x = *p660;
    z = *p664;
    xi = x >> 12;
    zi = z >> 12;

    if (xi + 0xc >= 0x100) {
        *pang = 0x8000 - *pang;
        *p660 = 0xf4000;
        func_02012718(0x1d4, *p660);
    } else if (xi - 0xc < 0) {
        *pang = 0x8000 - *pang;
        *p660 = 0xc000;
        func_02012718(0x1d4, *p660);
    }

    if (zi + 0xc > 0xc0) {
        *(u16 *)(c + 0x4686 + m) = -*(u16 *)(c + 0x4686 + m);
        *p664 = 0xb4000;
        func_02012718(0x1d4, *p660);
    } else if (zi - 0xc < -0xe0) {
        *(u16 *)(c + 0x4686 + m) = -*(u16 *)(c + 0x4686 + m);
        *p664 = -0xd4000;
        func_02012718(0x1d4, *p660);
    }

    zi = *p668;
    v = zi >> 9;
    if (v <= 0x1c)
        v = 0x1c;

    *(int *)(c + 0x4668 + m) -= v;
    pd = (int *)(c + 0x4668 + m);
    if (*(int *)(c + 0x4668 + m) <= 0) {
        *p668 = 0;
        *(u8 *)(c + idx * 0x30 + 0x4688) = 2;
    }

    func_ov006_020e5450((dScMgCurling2_c *)c, idx);
    StoneSpin(idx);

    v = *pd;
    w = -0xfa - ((0xc0 - (v >> 8)) * -0xfa) / 0xc0;
    p = v >> 7;
    if (p >= 0x7f)
        p = 0x7f;
    *(int *)(c + 0x467c + m) = func_02012468(*(int *)(c + 0x467c + m), 2, 0xe7, 7, p, w, func_020126e8(*p660), 0);
}


// @symbol _ZN15dScMgCurling2_c9StoneWaitEi
/* Stone state 0: armed, waiting. While a drag is on (unk_55b8 == 1) and
 * the drag point overlaps the stone, throw it at the latched angle and
 * power (clamped to the legal cone), mark it fast when the power is high,
 * then re-arm the spawn slot and separate it from any stone it landed on. */
void dScMgCurling2_c::StoneWait(int idx)
{
    if (unk_55b8 != 1) return;

    int v = (unk_5584 - mStone[idx].x) >> 12;
    int w = (unk_5588 - mStone[idx].y) >> 12;
    if (v < -0x2e) return;
    if (v > 0x2e) return;
    if (w < -0x14) return;
    if (w > 0x14) return;

    mStone[idx].state = 1;
    mStone[idx].angle = unk_55b2;
    mStone[idx].speed = unk_559c;

    if (unk_55b2 < 0x9800u ||
        unk_55b2 > 0xe800u) {
        if (unk_55b2 >= 0x4000u &&
            unk_55b2 <= 0x9800u) {
            unk_55b2 = 0x9800;
        } else {
            unk_55b2 = 0xe800;
        }
        mStone[idx].angle = unk_55b2;
    }

    if (unk_559c >= 0x3800) {
        mStone[idx].fast = 1;
    } else {
        mStone[idx].fast = 0;
    }

    spawning = 1;
    mSpawnTimer = 0;
    combo = 0;
    unk_55c0 = 0;
    unk_55c1 = 0;
    SeparateStones(idx);

    int vol = 0x7f;
    if (mStone[idx].fast == 0) vol = 0x3f;
    func_020126ac(0x1d3, 5, vol, 0,
                  func_020126e8(mStone[idx].x));
}


// @symbol _ZN15dScMgCurling2_c9NextStoneEv
/* Queue and deal the next stone: while spawning is set and the timer has
 * run out, activate stone[thrown] at the top of the rink, seed the house
 * stones on the first deal (the later deals play 0x1d7), then re-arm the
 * drag point and the draw flags. */
void dScMgCurling2_c::NextStone()
{
    char *self = (char *)this;
    if ((*((unsigned char *) (self + 0x55bb))) == 0)
    {
        return;
    }
    if ((*((unsigned short *) ((self + 0x5500) + 0xb4))) != 0)
    {
        *((unsigned short *) ((int)self + 0x55b4)) -= 1;
        if (*((short *) ((self + 0x5500) + 0xb4)) <= 0)
        {
            *((unsigned short *) ((self + 0x5500) + 0xb4)) = 0;
        }
        return;
    }
    *((unsigned char *) (self + 0x55bb)) = 0;
    {
        int n = *((unsigned char *) (self + 0x55ba));
        char *ip;
        char *p;
        char *q;
        if (n >= 5)
        {
            return;
        }
        ip = self + (n * 0x30);
        p = ip + 0x4000;
        *((unsigned char *) ((ip + 0x4000) + 0x689)) = 1;
        *((unsigned char *) ((ip + 0x4000) + 0x68a)) = 1;
        *((int *) (p + 0x660)) = 0x80000;
        *((int *) ((ip + 0x4000) + 0x664)) = 0x80000;
        q = ip + 0x4600;
        *((unsigned short *) (q + 0x80)) = 0;
        *((unsigned char *) ((ip + 0x4000) + 0x68b)) = 0;
        *((unsigned char *) ((ip + 0x4000) + 0x688)) = 0;
        *((unsigned char *) ((ip + 0x4000) + 0x68d)) = 0;
        *((unsigned char *) ((ip + 0x4000) + 0x68c)) = 0;
        if ((*((unsigned char *) (self + 0x55ba))) == 0)
        {
            SeedStones();
        }
        else
        {
            Sound::PlayBank2_2D(0x1d7);
        }
        *((unsigned char *) ((int)self + 0x55ba)) += 1;
        *((int *) (self + 0x5584)) = 0x80000;
        *((int *) (self + 0x5588)) = 0xb0000;
        *((unsigned char *) (self + 0x55b8)) = 0;
        *((unsigned char *) (self + 0x55b9)) = 1;
        ClearStoneFlags();
    }
}


// @symbol _ZN15dScMgCurling2_c10SeedStonesEv
/* Place the six house stones (mStone[5]..mStone[10]) at their fixed
 * positions, already at rest. */
void dScMgCurling2_c::SeedStones()
{
    struct E
    {
        char p[0x30];
    };
    int i;
    for (i = 0; i < 6; i++)
    {
        char *e = ((char *) (&((struct E *) this)[i + 5])) + 0x4000;
        *((unsigned char *) (e + 0x689)) = 1;
        *((unsigned char *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x68a)) = 1;
        *((int *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x660)) = data_ov006_0212e504[i] << 0xc;
        *((int *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x664)) = (-(data_ov006_0212e51c[i] + 0x20)) << 0xc;
        *((unsigned char *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x688)) = 2;
        *((int *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x668)) = 0;
        *((unsigned char *) ((((char *) (&((struct E *) this)[i + 5])) + 0x4000) + 0x68d)) = 1;
    }
}


// @symbol _ZN15dScMgCurling2_c15ClearStoneFlagsEv
/* Clear the +0x2c flag on every stone. */
void dScMgCurling2_c::ClearStoneFlags()
{
    int i;
    for (i = 0; i < 0xb; i++) {
        mStone[i].unk2c = 0;
    }
}


// @symbol _ZN15dScMgCurling2_c4IdleEv
/* Behavior state 4: terminal. */
void dScMgCurling2_c::Idle()
{
}


// @symbol _ZN15dScMgCurling2_c8EndRoundEv
/* Behavior state 3: the last stone has settled. Wait out the state timer,
 * clear the round flags, hide the stones, drop the prompt, hand the scene
 * to func_ov004_020b0a54 and park in state 4. */
void dScMgCurling2_c::EndRound()
{
    int i;
    if (mStateTimer != 0) {
        mStateTimer -= 1;
        if (*(s16 *)&mStateTimer <= 0)
            mStateTimer = 0;
        return;
    }

    if (func_ov004_020adbe0() != 0) {
        gameOver = 0;
        mState = 4;
        func_ov004_020b0a54(0x10);
    } else {
        mState = 4;
        func_ov004_020b0a54(0x10);
    }
    mPromptEnabled = 0;
    unk_55b9 = 0;
    for (i = 0; i < 0xb; i++) {
        mStone[i].visible = 0;
    }
    unk_55c3 = 0;
}


// @symbol _ZN15dScMgCurling2_c9NextThrowEv
/* Behavior state 2, reached once every stone rests. Age the score marks,
 * then either go back to Play after dealing the next stone or -- with all
 * five thrown -- set gameOver, sound the horn and advance to EndRound. */
#pragma push
#pragma opt_strength_reduction off
void dScMgCurling2_c::NextThrow()
{
    int i;
    AgeMarks();
    if (mStateTimer != 0) {
        mStateTimer -= 1;
        if (*(s16 *)&mStateTimer <= 0) mStateTimer = 0;
        return;
    }
    mState = 1;
    if (thrown >= 5) {
        mState = 3;
        mStateTimer = 0x40;
        gameOver = 1;
        Sound::PlayBank2_2D(0x1d8);
    }
    for (i = 0; i < 5; i++) {
        mMark[i].x = 0;
        mMark[i].y = 0;
        mMark[i].value = 0;
        mMark[i].countdown = 0;
        mMark[i].drawn = 0;
        mMark[i].enable = 0;
    }
    NextStone();
}
#pragma pop


// @symbol _ZN15dScMgCurling2_c4PlayEv
/* Behavior state 1: one frame of play. Enable the prompt on the first
 * pass, tick the sound throttle, run the drag-state handler, then step
 * every live stone through its own state handler (prev position latched
 * first, hit timer ticked). When all eleven are at rest, hand off to
 * NextThrow with the usual 0x40 delay. */
void dScMgCurling2_c::Play()
{
    char *c = (char *)this;
    if (*(u16*)(c + 0x55b6) != 0) {
        u16* q = (u16*)(c + 0x55b6);
        *q = *q - 1;
        return;
    }
    if (*(u8*)(c + 0xc4) == 0) {
        *(u8*)(c + 0xc3) = 1;
        *(u8*)(c + 0xc4) = 1;
        *(u16*)(c + 0xc0) = 0;
    }
    if (*(u8*)(c + 0x55bd) != 0) {
        u8* q = (u8*)(c + 0x55bd);
        *q = *q - 1;
    }
    (((C*)c)->*data_ov006_02141978[*(u8*)(c + 0x55b8)])();

    {
        int count = 0;
        int i = 0;
        char* p = c;
        for (; i < 0xb; i++, p += 0x30) {
            if (*(u8*)(p + 0x4689) != 0) {
                *(int*)(p + 0x466c) = *(int*)(p + 0x4660);
                *(int*)(p + 0x4670) = *(int*)(p + 0x4664);
                if (*(u16*)(p + 0x4680) != 0) {
                    u16* q = (u16*)(p + 0x4680);
                    *q = *q - 1;
                }
                (((C*)c)->*data_ov006_021419d8[*(u8*)(p + 0x4688)])(i);
                if (*(u8*)(p + 0x4688) != 2) count++;
            }
        }
        if (count != 0) return;
    }
    *(int*)(c + 0x5580) = 2;
    *(u16*)(c + 0x55b6) = 0x40;
}


// @symbol _ZN15dScMgCurling2_c10BeginRoundEv
/* Behavior state 0: (re)start a round -- full game reset, arm the spawn
 * slot, deal the first stone, then play. */
void dScMgCurling2_c::BeginRound()
{
    ResetGame();
    spawning = 1;
    mSpawnTimer = 0;
    unk_55c3 = 1;
    NextStone();
    mState = 1;
}


// @symbol _ZN15dScMgCurling2_c6ScrollEv
/* Pick the rink's scroll mode: 0xff in scrollIdx forces mode 0, otherwise
 * a random mode different from the current one. Apply the mode's BG2/BG0
 * offsets and set the BG0 flag in data_0209d45c unless the mode is 0. */
void dScMgCurling2_c::Scroll()
{
    int idx;

    if (scrollIdx == 0xff) {
        scrollIdx = 0;
    } else {
        idx = ((u32)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) * 4 >> 15;
        if (scrollIdx == idx) {
            int t = ((u32)RandomIntInternal(&data_0209d4b8) >> 16 & 0x7fff) * 3 >> 15;
            t += 1;
            idx += t;
            if (idx >= 4) idx -= 4;
        }
        scrollIdx = idx;
    }

    bg2x = data_ov006_0212e534[scrollIdx * 2];
    bg2y = data_ov006_0212e554[scrollIdx * 2];
    bg0x = data_ov006_0212e534[scrollIdx * 2 + 1];
    bg0y = data_ov006_0212e554[scrollIdx * 2 + 1];

    SetBg2Offset(0x80 - bg2x, 0x80 - bg2y);

    if (scrollIdx == 0) {
        data_0209d45c &= ~1;
        return;
    }
    SetBg0Offset(0x80 - bg0x, 0x80 - bg0y);
    data_0209d45c |= 1;
}


// @symbol _ZN15dScMgCurling2_c11ResetScrollEv
/* Mark the scroll mode unset so Scroll repicks it. */
void dScMgCurling2_c::ResetScroll()
{
    scrollIdx = 0xff;
}


// @symbol _ZN15dScMgCurling2_c9ResetGameEv
/* Clear the stones, the score marks and every game variable back to a
 * fresh round, then clear the collision values. */
void dScMgCurling2_c::ResetGame()
{
    char *c = (char *)this;
    int i;
    char *r = c;
    for (i = 0; i < 0xb; i++)
    {
        *((int *) (r + 0x4660)) = 0;
        *((int *) (r + 0x4664)) = 0;
        *((int *) (r + 0x4668)) = 0;
        *((int *) (r + 0x466c)) = 0;
        *((int *) (r + 0x4670)) = 0;
        *((int *) (r + 0x467c)) = 0;
        *((short *) (r + 0x4682)) = 0;
        *((short *) (r + 0x4684)) = 0;
        *((short *) (r + 0x4686)) = 0;
        *((unsigned char *) (r + 0x4688)) = 0;
        *((unsigned char *) (r + 0x4689)) = 0;
        *((unsigned char *) (r + 0x468a)) = 0;
        r += 0x30;
    }

    for (i = 0; i < 5; i++)
    {
        char *q = c + (i * ((unsigned long) 0x10));
        *((int *) (q + 0x4870)) = 0;
        *((int *) (q + 0x4874)) = 0;
        *((short *) (q + 0x4878)) = 0;
        *((short *) (q + 0x487a)) = 0;
        *((unsigned char *) (q + 0x487c)) = 0;
        *((unsigned char *) (q + 0x487d)) = 0;
    }

    *((short *) (c + 0x55b4)) = 0;
    *((unsigned char *) (c + 0x55ba)) = 0;
    *((int *) (c + 0x5584)) = 0;
    *((int *) (c + 0x5588)) = 0;
    *((int *) (c + 0x5594)) = 0;
    *((int *) (c + 0x5598)) = 0;
    *((int *) (c + 0x559c)) = 0;
    *((short *) (c + 0x55b2)) = 0;
    *((unsigned char *) (c + 0x55b8)) = 0;
    *((unsigned char *) (c + 0x55b9)) = 0;
    *((unsigned char *) (c + 0x55bb)) = 0;
    *((unsigned char *) (c + 0x55bc)) = 0;
    *((short *) (c + 0x55b6)) = 0;
    *((short *) (c + 0x55b0)) = 0;
    *((unsigned char *) (c + 0x55bd)) = 0;
    func_ov004_020adb1c(0);
    ClearValues();
}


// @symbol _ZN15dScMgCurling2_c13OnYoshiTryEatEi
/* Slot 18. Yoshi ate the puck: restart the round and re-arm the screen
 * blend and the frame counter. */
void dScMgCurling2_c::OnYoshiTryEat(int /* arg */)
{
    mState = 0;
    ResetGame();
    Scroll();
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4000050, 0, 0xd, 2, 0x10);
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4001050, 0, 4, 2, 0x10);
    unk_55ac = func_ov004_020adc1c();
}


// @symbol _ZN15dScMgCurling2_c6RenderEv
/* Slot 9. Per-frame draw: score marks, the throws counter, stones,
 * collision values, pieces and the drag cursor. */
s32 dScMgCurling2_c::Render()
{
    func_ov004_020b19f0((void *)func_ov004_020adc1c());
    DrawMarks();
    DrawCounter();
    DrawStones();
    DrawValues();
    DrawPieces();
    DrawCursor();
    return 1;
}


// @symbol _ZN15dScMgCurling2_c8BehaviorEv
/* Slot 6. One state-handler call through data_ov006_02141a18, then the
 * falling pieces and the collision values. */
s32 dScMgCurling2_c::Behavior()
{
    C *c = (C *)this;
    (c->*data_ov006_02141a18[mState])();
    StepPieces();
    AgeValues();
    return 1;
}


// @symbol _ZN15dScMgCurling2_c13InitResourcesEv
/* Slot 0. Load the rink and stone graphics for both engines, decompress
 * the board layouts, set up the blends, then reset into the first round
 * with a 0x40 state delay. */
s32 dScMgCurling2_c::InitResources()
{
    char *r6 = (char *)func_ov004_020adc74(&data_ov006_0213c5a0);
    void *f;
    void *c7;
    void *c8;
    volatile u16 sp4;
    volatile u16 sp6;

    if (r6 == 0) return 0;

    data_0209d45c |= 8;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 2;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1220;
    DecompressLZ16(r6, (void *)func_02054d88());

    f = (void *)LoadFile(0x45);
    _ZN2GX10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Deallocate(f);

    f = (void *)LoadFile(0x42);
    func_02056314(f, 0, 0x800);
    Deallocate(f);

    data_0209d45c |= 4;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 2;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x5420;

    f = (void *)LoadFile(0x41);
    {
        char *b = (char *)_ZN2G212GetBG2ScrPtrEv();
        sp4 = 0x4300;
        MultiStore16(sp4, b, 0x1000);
    }
    func_020563d4(f, 0, 0x800);

    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & ~3) | 2;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0x5620;
    {
        char *b = (char *)_ZN2G212GetBG0ScrPtrEv();
        sp6 = 0x4300;
        MultiStore16(sp6, b, 0x1000);
    }
    func_02056554(f, 0, 0x800);
    Deallocate(f);

    c7 = (void *)LoadFile(0xc7);
    c8 = (void *)LoadFile(0xc8);
    DecompressLZ16(c7, (void *)0x6400000);
    _ZN2GX11LoadOBJPlttEPKvjj(c8, 0, 0x100);

    data_0209d454 |= 4;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 2;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x814;
    DecompressLZ16(r6, (void *)_ZN3G2S13GetBG2CharPtrEv());

    f = (void *)LoadFile(0x45);
    _ZN3GXS10LoadBGPlttEPKvjj(f, 0x60, 0x1a0);
    Deallocate(f);

    f = (void *)LoadFile(0x43);
    func_02056374(f, 0, 0x800);
    Deallocate(f);

    Ov004_Deallocate(r6);

    DecompressLZ16(c7, (void *)0x6600000);
    _ZN3GXS11LoadOBJPlttEPKvjj(c8, 0, 0x100);
    Deallocate(c7);
    Deallocate(c8);

    ResetGame();
    ResetScroll();
    Scroll();
    spawning = 1;
    mSpawnTimer = 0;
    unk_55c3 = 1;
    NextStone();
    SeedPieces();
    mState = 1;
    func_ov004_020b04d0(0x20);
    mStateTimer = 0x40;
    unk_0a4 = 1;
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4000050, 0, 0xd, 2, 0x10);
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4001050, 0, 4, 2, 0x10);
    unk_55ac = func_ov004_020adc1c();
    return 1;
}
