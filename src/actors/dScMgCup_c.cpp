//cpp
/* Shell game (MG_CUP). Three cups shuffle on the sub screen; touch the one
 * whose unk_5462 matches unk_5468.
 *
 * Leftover: func_ov006_020dec88 calls the mangled Fix12 OAM::Render.
 *   include/OAM.h will not declare that overload.
 * Leftover: func_ov006_020ded00 keeps two spellings of frame. One shared
 *   spelling reuses the address and the body shrinks.
 * Leftover: func_ov006_020def80 keeps the 0x5000 bases. mFlags[i] / mAnim[i]
 *   come out 0xa0 bytes instead of 0xa4.
 * Leftover: StatePrepareShuffle re-reads mTimer as raw+0x5000+0x41c and
 *   decrements mSparkleShuffles through a byte pointer. The member form misses.
 * Leftover: StateShuffle's angle read-modify-write stays on raw+0x5400.
 *   mShuffleAngle += folds the address.
 * Leftover: Render's frame index stays ((int *)raw + k)[0x1510], and the
 *   cup x/y in that call stay two different spellings.
 * Leftover: Behavior's mAnim / mFrame / mTick loads stay (int) casts.
 * Leftover: the factory is still _ZN11dScMgBase_cC2Ev plus SysTracker C1.
 *   There is no dScMgCup_c constructor.
 */

#include "dScMgCup_c.h"
#include "types.h"
#include "decl_common.h"
#include "OamAttr.h"

/* data_ov006_0213c094: swap count, then two speed-row nibbles.
   StateSetup reads byte [kind * 2]. df024 reads .lo / .hi. */
typedef struct { u8 swaps; u8 lo:4; u8 hi:4; } RoundRow;

/* data_ov006_0213c0a8 is the three rest positions. 0213c0ac is one word
   later, so [i].x there is cup i's y. */
struct RestPos { s32 x, y; };

/* data_ov006_0213c0c0 is the three swap pairs. 0213c0c4 is one word later,
   so [i].a there is the second cup. */
struct IdxPair { s32 a, b; };

/* __sinit_ov006_021303d0 writes this eight-entry table. */
typedef void (dScMgCup_c::*PMF)();

/* One step of data_ov006_0213c0d8. list is an OamAttr run ending at attr3
   0xffff (func_ov006_020deed8). ticks == 0 means Behavior does not advance. */
struct AnimStep {
    OamAttr *list;
    int ticks;
};

namespace Sound {
    void PlayBank2_2D(unsigned int id);
}

namespace G2S {
    int GetBG2CharPtr();
    int GetBG0ScrPtr();
    int GetBG2ScrPtr();
}

namespace GXS {
    void LoadBGPltt(const void *data, unsigned int offset, unsigned int size);
    void LoadOBJPltt(const void *data, unsigned int offset, unsigned int size);
}

extern "C" {
extern void func_ov006_020dec5c(char *p, int a, int b);
extern int func_ov006_020deac8(char *p, int a, int b, int c);
extern void func_ov006_020ded84(char *p, int a, int b, int c, signed char e);
extern void func_ov006_020dec88(char *a);
extern void func_ov006_020ded00(int p);
struct OamAttr;
extern int data_ov006_0213c114[];
extern unsigned char data_ov006_0213c064[];
extern int data_ov006_0213c074[];
extern int _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int show, struct OamAttr* attr, int a, int b, int c, int d, int e, int f);
extern unsigned char DecIfAbove0_Byte(unsigned char *counter);
extern int func_02053200(int x);
extern int func_ov004_020af770(int a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6);
extern int data_0209e650;
extern RoundRow data_ov006_0213c094[];
extern s16 data_ov006_0213c0f4[];
extern IdxPair data_ov006_0213c0c0[];
extern IdxPair data_ov006_0213c0c4[];
extern RestPos data_ov006_0213c0a8[];
extern RestPos data_ov006_0213c0ac[];
extern int RandomIntInternal(int *seed);
extern void func_ov004_020b0a54(int c);
extern void func_ov006_020def80(char *self, int i);
extern void func_ov006_020c2594(void *c);
extern void func_ov004_020b1b78(void *c, int val);
extern void func_ov006_020c2664(char *c);
extern void func_ov006_020c2440(char *c);
extern void func_02012790(int x);
extern void func_ov004_020adb1c(int self);
extern unsigned char data_020a0e40[];
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0deb[];
extern unsigned char data_020a0dea[];
extern void FreeGfxSlotsById(int arg);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern void func_ov006_020deaf0(char* p, int key, int a, int b);
extern int func_ov006_020deb48(char *c, int a, int b, int d, signed char e);
extern short data_02082214[];
extern signed char data_ov006_0213c084[];
extern signed char data_ov006_0213c085[];
extern void func_ov006_020df024(char *o);
extern void func_ov006_020c2924(char *c);
/* Render spells cup y as this view. mCup[cup].y changes that call. */
struct P8 { int a; int b; };
void func_ov006_020debb4(char *a, int b);
void func_ov006_020deed8(int a0, void *a1, int a2, int a3, int a4, int a5);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void func_ov004_020b2574(int arg0, int arg1);
void func_ov004_020b1e34(void *a, int b, int c, int d);
extern char data_ov006_02139df4[];
extern PMF data_ov006_02141870[];
extern AnimStep *data_ov006_0213c0d8[];
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern int data_0208ee44;
extern void func_ov006_020c225c(char *);
extern int func_ov006_020c3050(char *);
extern void LoadCompressedFileAt(int, int);
extern void *LoadFile(int);
void _ZN8Particle10SysTrackerC1Ev(void *tracker);
void func_ov006_020c33dc(void *table);
void __cxa_vec_ctor(void *base, int count, int stride, void *ctor, void *dtor);
void func_ov006_020e0634(void);
void func_0203d738(void);
extern void *_ZTV19dScMgSingle3DBase_c[];
extern void *_ZTV10dScMgCup_c[];
}

#pragma defer_codegen off
/* Emit each function as it is read, so D1 lands ahead of D0. */

// @symbol _ZN10dScMgCup_cD1Ev
/* Reverse of the factory: cups, effects, then the model. The tracker and
   the base chain are the inlined dScMgSingle3DBase_c destructor. */
dScMgCup_c::~dScMgCup_c()
{
    __cxa_vec_cleanup(mCup, 3, 8, (void *)NullDestructor_0203d47c);
    __cxa_vec_cleanup(mFx, 0x20, 0x18, (void *)func_ov006_020deac4);
    func_ov006_020c3288((char *)&mModel);
}

extern "C" void func_ov006_020deac4(void)
{
}

extern "C" int func_ov006_020deac8(char *sprite, int, int, int)
{
    CupFx *fx = (CupFx *)sprite;
    int slot = 0;
    do {
        if (fx->active == 0)
            break;
        slot++;
        fx++;
    } while (slot < 0x20);
    return slot;
}

extern "C" void func_ov006_020deaf0(char *sprite, int cup, int dx, int dy)
{
    CupFx *fx = (CupFx *)sprite;
    int i;
    for (i = 0; i < 0x20; i++) {
        if (fx->active != 0) {
            if (cup == fx->cup) {
                func_ov006_020dec5c((char *)fx, dx, dy);
            }
        }
        fx++;
    }
}

extern "C" int func_ov006_020deb48(char *sprites, int kind, int x, int y, signed char cup) {
    int slot = func_ov006_020deac8(sprites, kind, x, y);
    if (slot >= 0 && slot < 0x20) {
        func_ov006_020ded84((char *)((CupFx *)sprites + slot), kind, x, y, cup);
        return slot;
    }
    return -1;
}

extern "C" void func_ov006_020debb4(char *sprite, int cup)
{
    CupFx *fx = (CupFx *)sprite;
    int i = 0;
    do {
        if (fx->active != 0 && cup == fx->cup)
            func_ov006_020dec88((char *)fx);
        i++;
        fx++;
    } while (i < 0x20);
}

extern "C" void func_ov006_020debfc(char *sprites)
{
    CupFx *fx = (CupFx *)sprites;
    int i;
    for (i = 0; i < 0x20; i++) {
        if (fx->active) func_ov006_020ded00((int)fx);
        fx++;
    }
}

extern "C" void func_ov006_020dec3c(char *sprite) {
    CupFx *fx = (CupFx *)sprite;
    int i;
    for (i = 0; i < 0x20; i++) {
        fx->active = 0;
        fx++;
    }
}

extern "C" void func_ov006_020dec5c(char* sprite, int dx, int dy)
{
    CupFx *fx = (CupFx *)sprite;
    if (fx->kind != 1)
        return;
    fx->x += dx;
    fx->y += dy;
}

extern "C" void func_ov006_020dec88(char* sprite)
{
    CupFx *fx = (CupFx *)sprite;
    int idx = fx->frame + fx->row * 6;
    /* Fix12 overload. OAM.h does not declare it: a by-value Fix12 homes
       r0-r3 on the stack. */
    _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(
        1,
        (struct OamAttr*)data_ov006_0213c074[data_ov006_0213c064[idx]],
        fx->x >> 12,
        fx->y >> 12,
        -1,
        -1,
        data_ov006_0213c114[idx],
        0);
}

extern "C" void func_ov006_020ded00(int sprite)
{
  *((int *) (sprite + 0)) += *((int *) (sprite + 8));
  *((int *) (sprite + 4)) += *((int *) (sprite + 0xc));
  if (*((unsigned char *) (sprite + 0x13)))
  {
    *((int *) (sprite + 4)) -= 6;
  }
  if (DecIfAbove0_Byte((unsigned char *) (sprite + 0x14)))
  {
    return;
  }
  *((unsigned char *) (sprite + 0x14)) = 3;
  /* Keep the two spellings of +0x12 apart: the ROM re-reads the byte after
     the decrement, and one shared spelling reuses the address instead. */
  *((signed char *) (sprite + 0x12)) -= 1;
  if (*((signed char *) sprite + 0x12) < 0)
  {
    *((signed char *) (sprite + 0x15)) = 0;
  }
}

extern "C" void func_ov006_020ded84(char* sprite, int kind, int x, int y, signed char cup)
{
    CupFx *fx = (CupFx *)sprite;
    if (kind == 2) {
        fx->kind = 0;
        fx->row = 1;
    } else {
        fx->kind = (unsigned char)kind;
        fx->row = 0;
    }
    fx->cup = cup;
    fx->active = 1;
    fx->x = x;
    fx->y = y;
    fx->vx = 0;
    fx->vy = 0;
    if (fx->row != 0)
        fx->frame = 7;
    else
        fx->frame = 5;
    fx->delay = 3;
}

extern "C" void func_ov006_020dedfc(char *raw, int anim, int frame, int cup)
{
    dScMgCup_c *self = (dScMgCup_c *)raw;
    if (anim == 6) {
        if (frame == 4) {
            func_02012718(0x1cf, self->mCup[cup].x);
        } else if (frame == 5 || frame == 0xb) {
            func_02012718(0x1d0, self->mCup[cup].x);
        }
    }
    if (anim == 5 && frame == 6 && cup == self->mRevealCup) {
        func_02012718(0x1ce, self->mCup[cup].x);
    }
    if (anim != 1 && anim != 4) return;
    if (frame != 4) return;
    Sound::PlayBank2_2D(0x1cc);
}

extern "C" void func_ov006_020deed8(int scene, void *list, int x, int y, int scale, int modeArg)
{
    OamAttr *entry;
    int mode;
    int sx;
    int sy;

    entry = (OamAttr *)list;
    sx = x >> 12;
    sy = y >> 12;
    mode = modeArg;
    scale = func_02053200(scale);

    for (;;) {
        int oam = func_ov004_020af770((int)entry, sx, sy, -1, -1, scale, 0);
        if (oam != 0) {
            if (mode == 2) {
                /* Loaded as one word; the shifts keep attr2 bits 12..15, the palette. */
                if ((unsigned)(*(int *)((char *)entry + 4) << 0x10) >> 0x1c == 3) {
                    int *p = (int *)(oam + 4);
                    *p = (*p & ~0xf000) | 0x4000;
                }
            }
        }
        if (entry->attr3 == 0xffff)
            break;
        entry++;
    }
}

extern "C" void func_ov006_020def80(char *raw, int i)
{
    unsigned char flag;
    char *row = raw + i;
    int cup;
    row += 0x5000;
    flag = *((unsigned char *) (row + 0x465));
    if (flag == 0) {
        flag = *((unsigned char *) (row + 0x462));
        if (flag != 0) {
            char *out = raw + (i << 2);
            out += 0x5000;
            *((int *) (out + 0x434)) = 4;
        } else {
            char *out = raw + (i << 2);
            out += 0x5000;
            *((int *) (out + 0x434)) = 1;
        }
        cup = i;
        goto epilogue;
    }
    flag = *((unsigned char *) (row + 0x462));
    if (flag != 0) {
        char *out = raw + (i << 2);
        out += 0x5000;
        *((int *) (out + 0x434)) = 5;
        if ((*((unsigned char *) ((raw + 0x5000) + 0x46d))) == 0xff) {
            if ((*((unsigned char *) ((raw + 0x5000) + 0x469))) != 1) {
                *((unsigned char *) ((raw + 0x5000) + 0x46d)) = (unsigned char) i;
            }
        }
        cup = i;
        goto epilogue;
    }
    {
        char *out = raw + (i << 2);
        out += 0x5000;
        *((int *) (out + 0x434)) = 2;
    }
    cup = i;

    /* Every path assigns cup before the shared epilogue; using i there, or
       assigning cup once at the top, emits a 0xa0-byte body. */
epilogue:
    {
        char *out = raw + (cup << 2);
        out += 0x5000;
        *((int *) (out + 0x440)) = 0;
        *((int *) (out + 0x44c)) = 0;
    }
}

extern "C" void func_ov006_020df024(char *raw)
{
    dScMgCup_c *self = (dScMgCup_c *)raw;
    u32 rnd = (u32)RandomIntInternal(&data_0209e650);
    u32 bits = rnd >> 0x10;
    int row;
    int rem;

    if (bits & 1) {
        row = data_ov006_0213c094[self->unk_5461].hi;
    } else {
        row = data_ov006_0213c094[self->unk_5461].lo;
    }

    row = row * 2;
    if (bits & 2)
        row += 1;

    self->mShuffleSpeed = data_ov006_0213c0f4[row];

    rem = (rnd >> 0x18) % 3;
    self->mSwap0 = data_ov006_0213c0c0[rem].a;
    self->mSwap1 = data_ov006_0213c0c4[rem].a;

    self->mMidX = (data_ov006_0213c0a8[self->mSwap1].x +
                   data_ov006_0213c0a8[self->mSwap0].x) / 2;
    self->mMidY = (data_ov006_0213c0ac[self->mSwap1].x +
                   data_ov006_0213c0ac[self->mSwap0].x) / 2;
    self->mHalfX = self->mMidX -
                   data_ov006_0213c0a8[self->mSwap0].x;
    self->mShuffleAngle = 0;

    {
        u8 kind = self->unk_5461;
        if (kind == 7) {
            if (self->unk_5460 == self->mFakeOutAt) {
                goto set1;
            }
        }
        if (kind < 7) {
            goto set0;
        }
        {
            u32 r2 = (u32)RandomIntInternal(&data_0209e650);
            if (((r2 >> 0x10) & 7) != 0) {
                goto set0;
            }
        }
    set1:
        self->mFakeOut = 1;
        return;
    set0:
        self->mFakeOut = 0;
        return;
    }
}

// @symbol _ZN10dScMgCup_c9StateIdleEv
void dScMgCup_c::StateIdle()
{
}

// @symbol _ZN10dScMgCup_c11StateFinishEv
void dScMgCup_c::StateFinish()
{
    mTimer -= 1;
    if (mTimer > 0) return;
    if (mCorrect != 0) {
        if (mHudScore < 0x270f) mHudScore += 1;
        if (mHudScore > unk_0b8) unk_0b8 = mHudScore;
        func_ov004_020b0a54(0);
        mState = 7;
    } else {
        if (unk_0a8 > 0) {
            OnYoshiTryEat(-1);
        } else {
            func_ov004_020b0a54(0x12);
            mState = 7;
        }
    }
}

// @symbol _ZN10dScMgCup_c11StateResultEv
#pragma push
#pragma opt_strength_reduction off
void dScMgCup_c::StateResult()
{
    int score;
    int i;

    mTimer = mTimer - 1;
    if (mTimer > 0) return;
    for (i = 0; i < 3; i++) {
        if (mFlags[i] == 0) {
            mFlags[i] = 1;
            func_ov006_020def80((char *)this, i);
        }
    }
    score = mHudScore;
    if (mCorrect != 0) {
        mTimer = 0x3c;
        func_ov006_020c2594(&mModel);
        score++;
    } else {
        func_ov004_020b1b78(this, 1);
        if (unk_0a8 > 0) {
            mTimer = 0x96;
            func_ov006_020c2664((char *)&mModel);
        } else {
            mTimer = 0x1e;
            func_ov006_020c2440((char *)&mModel);
        }
        func_02012790(0x12f);
    }
    func_ov004_020adb1c(score);
    mState = 6;
    mPromptEnabled = 0;
}
#pragma pop

// @symbol _ZN10dScMgCup_c11StateSelectEv
#pragma push
#pragma opt_common_subs off
#pragma opt_strength_reduction off
void dScMgCup_c::StateSelect()
{
    char *raw = (char *)this;
    int touched = 0;
    unsigned int idx = data_020a0e40[0];
    int cup;
    unsigned char touchX;
    unsigned char touchY;

    if (data_020a0de8[idx * 4] != 0 && data_020a0de9[idx * 4] != 0) touched = 1;
    if (touched == 0) return;

    {
        unsigned char* touch = &data_020a0de8[idx * 4];
        touchX = touch[2];
        touchY = touch[3];
    }

    for (cup = 0; cup < 3; cup++) {
        int dx = touchX - (mCup[cup].x >> 12);
        int dy = touchY - (mCup[cup].y >> 12);
        if (dx > 0x10 || dx < -0x28 || dy > 0x18 || dy < -0x20) continue;

        mFlags[cup] = 1;
        func_ov006_020def80(raw, cup);

        if (unk_5468 == unk_5462[cup]) {
            mCorrect = 1;
            mAnim[cup] = 6;
        } else {
            mCorrect = 0;
        }

        mState = 5;
        mTimer = 0x3c;
        FreeGfxSlotsById(0x1d);

        {
            unsigned int idx2 = data_020a0e40[0];
            unsigned char a1 = *(volatile unsigned char*)&data_020a0deb[idx2 * 4];
            unsigned char a2 = *(volatile unsigned char*)&data_020a0dea[idx2 * 4];
            mTouchLock = 1;
            mTouchX = a2;
            mTouchY = a1;
        }
        Sound::PlayBank2_2D(0x1cd);
        return;
    }
}
#pragma pop

// @symbol _ZN10dScMgCup_c17StateWaitForInputEv
void dScMgCup_c::StateWaitForInput()
{
    mTimer -= 1;
    if (mTimer > 0)
        return;
    func_ov004_020b0cac(0xf, 0x80, 0x38, 0, -1, 0xd);
    mState = 4;
}

// @symbol _ZN10dScMgCup_c12StateShuffleEv
#pragma push
#pragma opt_common_subs off
void dScMgCup_c::StateShuffle()
{
    char *raw = (char *)this;
    int oldX, oldY;

    {
        int v = mShuffleSpeed;
        int volume;
        if (v < 0) v = -v;
        volume = (v * 0x1f4) / 0x1000;
        if (volume >= 0x1f4) volume = 0x1f4;
        mShuffleSound = func_02012468(mShuffleSound, 2, 0x1cb, 2, 0, volume, 0, 0);
    }

    mShuffleAngle += mShuffleSpeed;

    {
        s16 speed = mShuffleSpeed;
        if ((speed >= 0 && mShuffleAngle >= 0x8000u) ||
            (speed < 0 && mShuffleAngle <= 0x8000u)) {
            mShuffleAngle = 0x8000;
        }
    }

    {
        int slot = mIds[*(int*)(raw + 0x542c)];
        int cosine = data_02082214[(mShuffleAngle >> 4) * 2 + 1];
        oldX = mCup[slot].x;
        oldY = mCup[slot].y;
        mCup[slot].x = *(int*)(raw + 0x5400)
            - (int)(((long long)cosine * *(int*)(raw + 0x5408) + 0x800) >> 12);
    }
    {
        int slot = mIds[*(int*)(raw + 0x542c)];
        mCup[slot].y = *(int*)(raw + 0x5404)
            - (int)((data_02082214[(mShuffleAngle >> 4) * 2] * 0x14000LL + 0x800) >> 12);
    }
    {
        int slot = mIds[*(int*)(raw + 0x542c)];
        mOnes[slot] = ((mCup[slot].y - *(int*)(raw + 0x5404)) >> 7) + 0x1000;
    }
    {
        int slot = mIds[*(int*)(raw + 0x542c)];
        func_ov006_020deaf0((char *)mFx, (u8)(s8)slot,
            mCup[slot].x - oldX,
            mCup[slot].y - oldY);
    }

    {
        int slot = mIds[*(int*)(raw + 0x5430)];
        int cosine = data_02082214[(mShuffleAngle >> 4) * 2 + 1];
        oldX = mCup[slot].x;
        oldY = mCup[slot].y;
        mCup[slot].x = *(int*)(raw + 0x5400)
            + (int)(((long long)cosine * *(int*)(raw + 0x5408) + 0x800) >> 12);
    }
    {
        int slot = mIds[*(int*)(raw + 0x5430)];
        mCup[slot].y = *(int*)(raw + 0x5404)
            + (int)((data_02082214[(mShuffleAngle >> 4) * 2] * 0x14000LL + 0x800) >> 12);
    }
    {
        int slot = mIds[*(int*)(raw + 0x5430)];
        mOnes[slot] = ((mCup[slot].y - *(int*)(raw + 0x5404)) >> 7) + 0x1000;
    }
    {
        int slot = mIds[*(int*)(raw + 0x5430)];
        func_ov006_020deaf0((char *)mFx, (u8)(s8)slot,
            mCup[slot].x - oldX,
            mCup[slot].y - oldY);
    }

    {
        char *state = raw + 0x5400;
        u16 angle = *(u16*)(state + 0x5c);
        if (angle == 0x8000) {
            int tb = mIds[*(int*)(raw + 0x5430)];
            int ta = mIds[*(int*)(raw + 0x542c)];
            mIds[*(int*)(raw + 0x542c)] = tb;
            mIds[*(int*)(raw + 0x5430)] = ta;
            unk_5460 -= 1;
            if (unk_5460 == 0) {
                *(int*)(raw + 0x541c) = 0x1e;
                mState = 3;
            } else {
                u32 r = RandomIntInternal(&data_0209e650);
                *(int*)(raw + 0x541c) = (r >> 8) % 0x18 + 1;
                mState = 1;
            }
        } else if (*(u8*)(raw + 0x546a) != 0) {
            s16 speed = *(s16*)(state + 0x5e);
            if ((speed >= 0 && angle >= 0x5555u) || (speed < 0 && angle <= 0xaaabu)) {
                /* The stores respell the full offset from raw (a cached base
                   also relinks to the wrong data symbol), and the angle bump
                   stays a plain read-modify-write: `+=` folds the address. */
                speed = -speed;
                mShuffleSpeed = speed;
                mShuffleAngle = mShuffleAngle + 0x8000;
                {
                    int tb = mIds[*(int*)(raw + 0x5430)];
                    int ta = mIds[*(int*)(raw + 0x542c)];
                    mIds[*(int*)(raw + 0x542c)] = tb;
                    mIds[*(int*)(raw + 0x5430)] = ta;
                }
                *(u8*)(raw + 0x546a) = 0;
            }
        }
    }

    if (*(u8*)(raw + 0x546b) != 0) {
        int cnt = *(int*)(raw + 0x541c);
        if ((cnt & 3) == 0) {
            int i;
            {
                int slot = mIds[*(int*)(raw + 0x542c)];
                i = (cnt >> 2) & 7;
                func_ov006_020deb48((char *)mFx, 2,
                    mCup[slot].x + (data_ov006_0213c084[i * 2] << 12),
                    mCup[slot].y + (data_ov006_0213c085[i * 2] << 12),
                    slot);
            }
            {
                int slot = mIds[*(int*)(raw + 0x5430)];
                func_ov006_020deb48((char *)mFx, 2,
                    mCup[slot].x + (data_ov006_0213c084[i * 2] << 12),
                    mCup[slot].y + (data_ov006_0213c085[i * 2] << 12),
                    slot);
            }
        }
    } else {
        int cnt = *(int*)(raw + 0x541c);
        if (cnt & 1) {
            int i;
            {
                int slot = mIds[*(int*)(raw + 0x542c)];
                i = (cnt >> 1) & 7;
                func_ov006_020deb48((char *)mFx, 1,
                    mCup[slot].x + (data_ov006_0213c084[i * 2] << 12),
                    mCup[slot].y + (data_ov006_0213c085[i * 2] << 12),
                    slot);
            }
            {
                int slot = mIds[*(int*)(raw + 0x5430)];
                func_ov006_020deb48((char *)mFx, 1,
                    mCup[slot].x + (data_ov006_0213c084[i * 2] << 12),
                    mCup[slot].y + (data_ov006_0213c085[i * 2] << 12),
                    slot);
            }
        }
        if ((*(int*)(raw + 0x541c) & 3) == 0) {
            {
                int slot = mIds[*(int*)(raw + 0x542c)];
                func_ov006_020deb48((char *)mFx, 0,
                    mCup[slot].x - 0xe000,
                    mCup[slot].y + 0x1c000,
                    slot);
            }
            {
                int slot = mIds[*(int*)(raw + 0x5430)];
                func_ov006_020deb48((char *)mFx, 0,
                    mCup[slot].x - 0xe000,
                    mCup[slot].y + 0x1c000,
                    slot);
            }
        }
    }

    *(int*)(raw + 0x541c) += 1;
}
#pragma pop

// @symbol _ZN10dScMgCup_c19StatePrepareShuffleEv
void dScMgCup_c::StatePrepareShuffle()
{
    char *raw = (char *)this;
    int *countdown = (int *)(raw + 0x541c);
    *countdown = *countdown - 1;
    /* Same word as mTimer. The 0x5000 split is a second load; mTimer > 0 misses. */
    if (*(int *)(raw + 0x5000 + 0x41c) > 0) return;
    func_ov006_020df024(raw);
    if (*(unsigned char *)(raw + 0x5000 + 0x46b) != 0) {
        unsigned char *pending = (unsigned char *)(raw + 0x546b);
        *pending = *pending - 1;
    }
    mState = 2;
}

// @symbol _ZN10dScMgCup_c10StateSetupEv
void dScMgCup_c::StateSetup()
{
    int i;
    int score;
    unsigned int rnd;
    unsigned int hi;
    unsigned int kind;
    unsigned int picked;

    mTimer--;
    if (mTimer > 0) {
        return;
    }

    for (i = 0; i < 3; i++) {
        mFlags[i] = 0;
        func_ov006_020def80((char *)this, i);
    }

    score = mHudScore;
    if (score < 0xa) {
        unk_5461 = (unsigned char)score;
    } else {
        rnd = (unsigned int)RandomIntInternal(&data_0209e650);
        hi = rnd >> 16;
        kind = (hi % 5) + 5;
        unk_5461 = (unsigned char)kind;
    }

    unk_5460 = ((unsigned char *)data_ov006_0213c094)[unk_5461 * 2];

    if (score > 3) {
        rnd = (unsigned int)RandomIntInternal(&data_0209e650);
        if (rnd & 1) {
            unk_5468 = 1;
        } else {
            unk_5468 = 2;
        }
    } else if (score == 3) {
        unk_5468 = 2;
    }

    mSparkleShuffles = 0;
    mCorrect = 0;
    mState = 1;
    mTimer = 0x1e;
    mRevealCup = 0xff;
    mShuffleSound = 0;
    FreeGfxSlotsById(0x1d);

    rnd = (unsigned int)RandomIntInternal(&data_0209e650);
    picked = (rnd % 10) + 1;
    mFakeOutAt = (unsigned char)picked;

    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
}

// @symbol _ZN10dScMgCup_c9Virtual50Ev
/* Slot 20: the win animation on mModel. */
void dScMgCup_c::Virtual50()
{
    func_ov006_020c2594(&mModel);
}

#pragma push
#pragma opt_strength_reduction off
#pragma opt_common_subs off
void dScMgCup_c::OnYoshiTryEat(int msg)
{
    int i;

    if (msg == 3 || msg == 0x12) {
        if (msg == 3) {
            mHudScore = 0;
        } else {
            mHudScore = 0;
            func_ov004_020adb1c(mHudScore);
        }
        unk_0a8 = 2;
        unk_0ac = unk_0a8;
        unk_5462[0] = 1;
        unk_5462[1] = 0;
        if (mHudScore >= 3) {
            unk_5462[2] = 2;
        } else {
            unk_5462[2] = 0;
        }
        unk_5468 = 1;
    } else if (msg == 0) {
        unk_0a8 = 2;
        unk_0ac = unk_0a8;
        if (mHudScore == 3) {
            unk_5462[0] = 1;
            unk_5462[1] = 0;
            unk_5462[2] = 2;
        }
    }

    for (i = 0; i < 3; i++) {
        if (unk_5462[i] != 0) {
            mAnim[i] = 3;
        } else {
            mAnim[i] = 0;
        }
        mFrame[i] = 0;
        mTick[i] = 0;
    }

    mTimer = 0x3c;
    mState = 0;
    mTouchLock = 0;
    func_ov006_020c2924((char *)&mModel);

    unk_0bc = mHudScore;
    if (unk_0bc > 0x270e) {
        unk_0bc = 0x270e;
    }
    func_ov004_020b0cac(0xd, 0x80, 0xa0, 1, -1, 0xd);
}
#pragma pop

// @symbol _ZN10dScMgCup_c6RenderEv
/* Slot 9: draws the three cups back to front, the deepest first. */
s32 dScMgCup_c::Render()
{
    char *raw = (char *)this;
    int order[3];
    int i;
    int limit;
    int j;
    int cur;
    int next;
    int k;
    int cup;

    for (i = 0; i < 3; i++) {
        order[i] = i;
    }

    for (limit = 3; limit > 1; limit--) {
        for (j = 0; j < limit - 1; j++) {
            next = order[j + 1];
            cur = order[j];
            if (mCup[cur].y < mCup[next].y) {
                order[j] = next;
                order[j + 1] = cur;
            }
        }
    }

    /* Keep both index towers in the call below: spelled as unk_5440[k] or
       as a flat offset, the compiler recomputes the address and the code
       changes. */
    for (k = 0; k < 3; k++) {
        cup = order[k];
        func_ov006_020debb4((char *)mFx, (char)cup);
        func_ov006_020deed8((int)raw,
            (void *)data_ov006_0213c0d8[mAnim[cup]][((int*)raw + k)[0x1510]].list,
            *(int*)(raw + cup * 8 + 0x53e8),
            ((struct P8*)raw + cup)[0xa7d].b,
            mOnes[cup],
            unk_5462[cup]);
    }

    if (mState == 4 || mState == 5) {
        Hud_RenderSprite(data_ov006_02139df4, 0x92, 0x20,
            (unk_5468 == 2) ? 4 : -1, -1);
    }

    func_ov004_020b2574(unk_0a8, 1);
    func_ov004_020b1e34(raw, 0xe0, 0x14, 1);
    func_ov006_020c29dc((char *)&mModel);
    return 1;
}

// @symbol _ZN10dScMgCup_c8BehaviorEv
/* Slot 6: runs the current state from the table, then advances each cup's
   animation. The `(int)` casts on the three per-cup arrays steer the address
   arithmetic; removing them changes the code. */
#pragma push
#pragma opt_strength_reduction off
s32 dScMgCup_c::Behavior()
{
    char *raw = (char *)this;
    int i;
    (this->*data_ov006_02141870[mState])();
    for (i = 0; i < 3; i++) {
        AnimStep *anim = &data_ov006_0213c0d8[*(int *)(((int)raw + i * 4 + 0x5434))][*(int *)(raw + i * 4 + 0x5440)];
        int length = anim->ticks;
        if (length != 0) {
            *(int *)(((int)raw + i * 4 + 0x544c)) += 1;
            if (*(int *)(((int)raw + i * 4 + 0x544c)) >= length) {
                *(int *)(((int)raw + i * 4 + 0x544c)) = 0;
                *(int *)(((int)raw + i * 4 + 0x5440)) += 1;
                func_ov006_020dedfc(raw, *(int *)(((int)raw + i * 4 + 0x5434)),
                                    *(int *)(((int)raw + i * 4 + 0x5440)), i);
            }
        }
    }
    func_ov006_020debfc((char *)mFx);
    func_ov006_020c2b8c((char *)&mModel);
    return 1;
}
#pragma pop

// @symbol _ZN10dScMgCup_c13InitResourcesEv
/* Slot 0: sets up the sub-screen backgrounds, loads the board graphics,
   places the three cups and starts a game through OnYoshiTryEat(3). */
s32 dScMgCup_c::InitResources()
{
    void *file;

    data_0209d45c = 0x11;
    func_ov006_020c225c((char *)pad_4660);
    if (func_ov006_020c3050((char *)&mModel) == 0)
        return 0;

    {
        volatile u16 *bg = (volatile u16 *)0x04001008;
        *bg = (*bg & 0x43) | 0x2214;
        *bg = *bg & ~0x40;
        *bg = *bg & ~3;
        SetSubBg0Offset(0, 0);
    }

    data_0209d454 |= 1;
    {
        volatile u16 *bg = (volatile u16 *)0x0400100c;
        *bg = (*bg & 0x43) | 0x414;
        *bg = *bg & ~0x40;
        *bg = (*bg & ~3) | 3;
        SetSubBg2Offset(0, 0);
    }

    data_0209d454 |= 4;

    LoadCompressedFileAt(0x2b, G2S::GetBG2CharPtr() + 0x4000);
    file = LoadFile(0x2c);
    GXS::LoadBGPltt(file, 0x60, 0x1a0);
    Deallocate(file);
    LoadCompressedFileAt(0x2a, G2S::GetBG0ScrPtr());
    LoadCompressedFileAt(0x29, G2S::GetBG2ScrPtr());
    LoadCompressedFileAt(0xc5, 0x6600000);
    file = LoadFile(0xc6);
    GXS::LoadOBJPltt(file, 0, 0xa0);
    Deallocate(file);

    data_0208ee44 = 1;
    func_ov004_020b6808();
    data_ov004_020bc880 = 0x80;
    data_ov004_020bc884 = 0x88;
    data_ov004_020bc8a8 = 0x40;
    data_ov004_020bc898 = 0xa0;
    data_ov004_020bc86c = 0xc0;
    data_ov004_020bc8a4 = 0xa0;

    {
        int i;
        for (i = 0; i < 3; i++) {
            mCup[i].x = data_ov006_0213c0a8[i].x;
            mCup[i].y = data_ov006_0213c0a8[i].y;
            mOnes[i] = 0x1000;
            mIds[i] = i;
            mFlags[i] = 1;
        }
    }

    OnYoshiTryEat(3);
    func_ov006_020dec3c((char *)mFx);
    return 1;
}

// @symbol dScMgCup_c_classInit
/* No dScMgCup_c constructor. Base ctor, intermediate vtable, the tracker,
   then this vtable, the model, the effects, the cups. */
extern "C" void *dScMgCup_c_classInit()
{
    dScMgCup_c *scene = (dScMgCup_c *)_ZN7fBase_cnwEj(sizeof(dScMgCup_c));
    if (scene) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(void **)scene = _ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(&scene->mSysTracker);
        *(void **)scene = _ZTV10dScMgCup_c + 2;
        func_ov006_020c33dc(&scene->mModel);
        __cxa_vec_ctor(scene->mFx, 0x20, 0x18,
                      (void *)func_ov006_020e0634,
                      (void *)func_ov006_020deac4);
        __cxa_vec_ctor(scene->mCup, 3, 8,
                      (void *)func_0203d738,
                      (void *)NullDestructor_0203d47c);
    }
    return scene;
}

// @symbol func_ov006_020e0634
extern "C" void func_ov006_020e0634()
{
}
