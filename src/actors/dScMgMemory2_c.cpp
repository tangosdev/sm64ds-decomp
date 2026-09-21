//cpp
/* Memory Master: deal the cards, move the player markers and judge pairs.
 * Round, result and card states control the reveal, selection and reward phases.
 * The cartridge RTTI names this scene dScMgMemory2_c.
 *
 * This promoted TU owns 52 text functions, including both destructor variants.
 * Source order and local optimization pragmas preserve their measured emission.
 * Some loops retain their offset views after bounded ordinary-member probes.
 * See notes/experiments/pr2875-source-repair-0920.json for those alternatives.
 */

#include "dScMgMemory2_c.h"
#include "Sound.h"
#include "types.h"
#include "common.h"
#include "decl_common.h"

/* Remaining legacy views describe loop windows over the scene layout.
 * They are not recovered original class definitions. */

typedef void (dScMgMemory2_c::*dScMgMemory2_cState)();
typedef void (dScMgMemory2_c::*dScMgMemory2_cCardState)(int);
extern "C" dScMgMemory2_cCardState data_ov006_02142408[];

/* shadow typedef 'Car' */
typedef struct Car {
    /* 0x00 */ s32 x;
    /* 0x04 */ u8 unk04[0x12 - 0x04];
    /* 0x12 */ u8 b12;
    /* 0x13 */ u8 b13;
    /* 0x14 */ u8 b14;
    /* 0x15 */ u8 b15;
    /* 0x16 */ u8 b16;
    /* 0x17 */ u8 b17;
} Car;

/* shadow typedef 'Ctx' */
typedef struct Ctx {
    /* 0x0000 */ Car raw[1];
    /* 0x0018 */ u8 unk18[0x51a8 - 0x18];
    /* 0x51a8 */ Car cars[25];
    /* 0x5400 */ u8 unk5400[0xa];
    /* 0x540a */ u8 mode;
} Ctx;

/* shadow struct 'B' */
struct B {
    char pad[0xb4];
    int b4;
    int b8;
};

/* shadow typedef 'struct' */
typedef struct {
    char _pad0[0x13];
    u8 done;     /* +0x13 */
    char _pad1[4];
} Slot; /* 0x18 */

/* shadow typedef 'Work' */
typedef struct {
    char _pad0[0x51a8];
    Slot slots[23];   /* 0x51a8 */
    char _pad1[8];
    int state;        /* 0x53d8 */
    char _pad2[0xa];
    short ready;      /* 0x53e6 */
    short count;      /* 0x53e8 */
    u16 total;        /* 0x53ea */
    char _pad3[0x1e];
    u8 mode;          /* 0x540a */
} Work;

extern "C" dScMgMemory2_cState data_ov006_021423c0[];
extern "C" dScMgMemory2_cState data_ov006_021423e0[];
extern "C" dScMgMemory2_cState data_ov006_02142440[];

/* shadow namespace 'GX' */
namespace GX { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

/* shadow namespace 'GXS' */
namespace GXS { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

extern "C" {
extern void func_ov004_020b1e34(void *thiz, int a, int b, int c);
extern void func_ov004_020b0d8c(void *c, int arg1, int arg2);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern s16 data_02082214[];
extern "C" unsigned int func_02012790(unsigned int x);
extern "C" void func_ov004_020b5dd4(void);
extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern u16 data_ov006_0213d45c[];
extern void *data_ov006_02142490[];
extern s32 data_ov006_0212e930[];
extern unsigned short data_ov006_0213d344[];
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern void func_ov004_020ad79c(int a, int b);
extern u16* data_ov006_0213d338[];
extern int data_ov006_0212e8e8[];
extern int data_ov006_0212e8f4[];
extern void func_ov004_020b56c8(char* p);
extern short data_ov004_020bf9e4;
extern void func_ov004_020b67f8(void);
extern void func_ov004_020b0a54(int);
extern void func_ov004_020adb1c(int self);
extern void func_ov006_020c0c80(void *c);
extern void func_ov006_020c0d68(void *c);
extern void func_ov006_020c1604(char *c, int unused, short count, int ready);
extern int data_ov006_0212e948[];
extern int data_ov006_0212e93c[];
extern int data_ov006_0212e924[];
extern int data_ov006_0212e918[];
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern int data_ov006_0212e900[];
extern int data_ov006_0212e90c[];
extern void FreeGfxSlotsById(int x);
extern void func_ov006_020c1764(char *p);
// recovered name: dScMgMemory2_c_OnGroundPounded
/* recovered: renamed to Class_Method */
/* dScMgMemory2_c::OnGroundPounded - recovered from vtable slot identity */
// recovered name: dScMgMemory2_c_OnTurnIntoEgg
/* dScMgMemory2_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern int func_ov006_020c1718(int* p);
// recovered name: dScMgMemory2_c_OnYoshiTryEat_020f7394
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
declaration exactly, or mwcc appends a slot instead of overriding. */
extern void func_ov004_020b66d4(void);
/* data_ov004_020beb68 is declared `void *` by dScMgBase_c.h; cast at the use site. */
void func_ov006_020c0aa8(char *);
void func_ov004_020b1bc8(char *, int, int, int);
void func_ov006_020c1804(char *);
extern "C" void func_ov004_020b65e4(void);
extern "C" void func_ov006_020c19d0(char *p);
void func_ov006_0210a534(void);
s32 GetGameLanguage(void);
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void Deallocate(void *ptr);
int func_ov006_020c1a88(void *c);
int func_ov004_020ad8b8(void);
int func_ov004_020ad878(void);
void func_ov004_020b04d0(int v);
void func_ov004_020b682c(void);
extern u8 data_0209d45c;
extern u8 data_0209d454;
}

#pragma defer_codegen off

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN14dScMgMemory2_cD1Ev, 0x020f5564, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMemory2_cD1Ev
/* Member destruction releases mShared before the inherited scene destructor.
 * CodeWarrior emits both D1 and D0 from the single definition below. */
dScMgMemory2_c::~dScMgMemory2_c()
{
    /* mShared is destroyed automatically before the inherited scene. */
}
/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN14dScMgMemory2_cD0Ev, 0x020f55b8, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMemory2_cD0Ev
/* Compiler-emitted deleting variant of the destructor above. */
/* D1 is emitted with D0 from the single destructor definition above. */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov006_020f5620, 0x020f5620, size 0x3c */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::DrawMessage()
{
    char *thiz = (char *)this;
    if (mMessageVisible == 0) return;
    func_ov004_020b1e34(thiz, 0xe0, 0x14, 1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov006_020f565c, 0x020f565c, size 0x38 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::DrawCursor() {
    char *c = (char *)this;
    if (mCursor.enabled == 0) return;
    func_ov004_020b0d8c(c, 0xe0, 0xa0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov006_020f5694, 0x020f5694, size 0x64 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::UpdateCursor() {
  char *c = (char *)this;
  if (mCursor.visible == 0) return;
  {
    unsigned short* h = (unsigned short*)(c + 0x53cc);
    *h = *h + 1;
    if (*h < 0x14) return;
    *h = 0;
  }
  {
    unsigned char* p = (unsigned char*)(c + 0x53d1);
    *p = *p + 1;
    *p = *p & 1;
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov006_020f56f8, 0x020f56f8, size 0x34 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::ShowCursor()
{
    mCursor.visible = 1;
    mCursor.enabled = 1;
    mCursor.x = 966656;
    mCursor.y = 688128;
    mCursor.angle = 0;
    mCursor.frame = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov006_020f572c, 0x020f572c, size 0x14 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::HideCursor()
{
    mCursor.visible = 0;
    mCursor.enabled = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_020f5740, 0x020f5740, size 0x4 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::PlayerWait(int /* player */)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov006_020f5744, 0x020f5744, size 0x18c */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::PlayerMove(int i)
{
  char *base = (char *)this;
  int n = i * 0x14;
  int tx = (i << 4) + 0xc;
  int tz = -0x2c;
  u16 *new_var;
  int dx;
  int dz;
  dx = tx - ((*((int *) ((base + 0x5388) + n))) >> 12);
  dz = tz - ((*((int *) ((base + 0x538c) + n))) >> 12);
  *((s16 *) ((base + 0x5394) + n)) = _ZN4cstd5atan2E5Fix12IiES1_(dz, dx);
  *((int *) ((base + 0x5390) + n)) = (*((int *) ((base + 0x5390) + n))) + 0x200;
  new_var = &(*((u16 *) ((base + 0x5394) + n)));
  *((int *) ((base + 0x5388) + n)) = (*((int *) ((base + 0x5388) + n))) + ((int) (((((s64) data_02082214[(((*new_var) >> 4) << 1) + 1]) * (*((int *) ((base + 0x5390) + n)))) + 0x800) >> 0xc));
  *((int *) ((base + 0x538c) + n)) = (*((int *) ((base + 0x538c) + n))) + ((int) (((((s64) data_02082214[((*new_var) >> 4) << 1]) * (*((int *) ((base + 0x5390) + n)))) + 0x800) >> 0xc));
  dx = tx - ((*((int *) ((base + 0x5388) + n))) >> 12);
  dz = tz - ((*((int *) ((base + 0x538c) + n))) >> 12);
  if ((dx < (-3)) || (dx > 3))
  {
    return;
  }
  if ((dz < (-3)) || (dz > 3))
  {
    return;
  }
  *((int *) ((base + 0x5388) + n)) = tx << 12;
  *((int *) ((base + 0x538c) + n)) = tz << 12;
  *((unsigned char *) ((base + n) + 0x539a)) = 2;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_020f58d0, 0x020f58d0, size 0x6c */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::PlayerDrop(int i) {
    short *e = &mPlayers[i].delay;
    if (*(unsigned short*)e != 0) {
        *e = *(unsigned short*)e - 1;
        if (*e < 0) *e = 0;
        return;
    }
    mPlayers[i].x = 0xc000;
    mPlayers[i].y = 0xc000;
    mPlayers[i].unk_11 = 1;
    mPlayers[i].state = 1;
    mPlayers[i].speed = 0xc00;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov006_020f593c, 0x020f593c, size 0x84 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::InitPlayers()
{
    char *self = (char *)this;
    int n;
    int i;
    int angle;
    char *entry;

    if (data_ov004_020beb68 != 0)
        n = *(int *)((char *)data_ov004_020beb68 + 0xa8);
    else
        n = 0;
    if (n >= 5)
        n = 5;

    entry = self;
    i = 0;
    mMaxMisses = (unsigned char)n;
    if (n <= 0)
        return;

    angle = 0;
    do {
        *(unsigned char *)(entry + 0x5398) = 1;
        *(unsigned char *)(entry + 0x539a) = 0;
        *(short *)(entry + 0x5396) = angle;
        i++;
        entry += 0x14;
        angle += 0x10;
    } while (i < n);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov006_020f59c0, 0x020f59c0, size 0xa4 */
/* -------------------------------------------------------------------------- */
void func_ov004_020ad79c(int r0arg, int r1arg);
inline int *inline_fn(char *arg0)
{
  return (int *) arg0;
}

void dScMgMemory2_c::CheckFinished()
{
  char *c = (char *)this;
  if ((*((unsigned char *) (c + 0x5405))) >= (*((unsigned short *) (c + 0x53ea))))
  {
    *((short *) (c + 0x53e4)) = 0x40;
    *inline_fn(c + 0x53d4) = 3;
    *inline_fn(c + 0x53d8) = 0;
    func_ov004_020ad79c(((5 - (*((unsigned char *) (c + 0x5408)))) * 5) + (*inline_fn(c + 0xa8)), (*inline_fn(c + 0xb4)) + 1);
  }
  if ((*((unsigned char *) (c + 0x5408))) >= (*((unsigned char *) (c + 0x5409))))
  {
    *((short *) (c + 0x53e4)) = 0x40;
    *inline_fn(c + 0x53d4) = 3;
    *inline_fn(c + 0x53d8) = 0;
    func_ov004_020ad79c(*inline_fn(c + 0xa8), (*inline_fn(c + 0xb4)) - 1);
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov006_020f5a64, 0x020f5a64, size 0x134 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::JudgePair()
{
    int a, b;
    unsigned char *sa, *sb;
    if (mSelectedCount < 2) return;
    a = mSelectedCards[0];
    sa = &mCards[a].state;
    b = mSelectedCards[1];
    if (*sa != 4) return;
    sb = &mCards[b].state;
    if (*sb != 4) return;
    if (mCards[a].value == mCards[b].value) {
        mCards[a].active = 0;
        mCards[a].visible = 0;
        mCards[b].active = 0;
        mCards[b].visible = 0;
        func_02012790(0x26);
        Sound::PlayBank2_2D(0x13d);
        mPairsFound += 1;
        mSelectedCount = 0;
    } else {
        func_02012790(0xe);
        Sound::PlayBank2_2D(0x13e);
        mMisses += 1;
        func_ov004_020b5dd4();
        if (mMisses < mMaxMisses) {
            *sa = 5;
            *sb = 5;
            mSelectedCount = 0;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_020f5b98, 0x020f5b98, size 0xa8 */
/* -------------------------------------------------------------------------- */
// 6f: keep constant live / flip coloring
#pragma push
#pragma opt_propagation off
void dScMgMemory2_c::DrawCards()
{
    char *a0 = (char *)this;
    int i;
    int k, flag;
    char *p;

    /* p walks 20 dMeter_c-sprite records at a0+0x51a8, stride 0x18.
       data_ov006_0213d45c is indexed as [id][5]. */
    p = a0;
    i = 0;
    do {
        if (*(u8 *)(p + 0x51ba) != 0) {
            flag = 0;
            if (mState != 2)
                flag = 1;
            k = data_ov006_0213d45c[*(u8 *)(p + 0x51b8) * 5
                                    + *(u8 *)(p + 0x51bd)];
            Hud_RenderSprite(data_ov006_02142490[k],
                             *(int *)(p + 0x51a8) >> 12,
                             *(int *)(p + 0x51ac) >> 12, -1, flag);
        }
        i += 1;
        p += 0x18;
    } while (i < 20);
}
#pragma pop

/* -------------------------------------------------------------------------- */
/* ROM ordinal 14 -- func_ov006_020f5c40, 0x020f5c40, size 0x74 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::UpdateCards()
{
    int i;
    for (i = 0; i < 0x14; i++) {
        if (mCards[i].active)
            (this->*data_ov006_02142408[mCards[i].state])(i);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_020f5cb4, 0x020f5cb4, size 0x12c */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::CardFlyAway(int idx)
{
    Ctx *ctx = (Ctx *)this;
    s32 lim;
    s32 rem;
    s32 k;
    s32 *px;
    s32 n;
    u8 *pb;

    ctx->cars[idx].x -= 0x10000;
    if (ctx->cars[idx].b16 == 0) {
        lim = data_ov006_0212e930[ctx->mode];
        rem = idx;
        if (idx >= lim) {
            n = lim;
            do {
                rem -= n;
            } while (rem >= n);
        }
        if (rem == 0) {
            ctx->cars[idx].b16 = 1;
            return;
        }
        n = rem + 1;
        k = 1;
        if (n > 1) {
            do {
                px = (s32 *)((u8 *)&ctx->raw[idx] + 0x51a8);
                pb = (u8 *)&ctx->raw[idx] + 0x51be;
                if (ctx->cars[idx - k].b13 != 0) {
                    if (((*px - ctx->cars[idx - k].x) >> 12) <= 4) {
                        *pb = 1;
                        ctx->cars[idx - k].b14 = 6;
                    }
                }
                k++;
            } while (k < n);
        }
    }
    if ((ctx->cars[idx].x >> 12) > -0x18) {
        return;
    }
    *((u8 *)ctx + idx * 24 + 0x51bb) = 0;
    *((u8 *)ctx + idx * 24 + 0x51ba) = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 16 -- func_ov006_020f5de0, 0x020f5de0, size 0x90 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::CardFlipDown(int i){
    unsigned short cnt;
    mCards[i].animTimer += 1;
    cnt = mCards[i].animTimer;
    if(cnt < (data_ov006_0213d344[mCards[i].frame] & 0xff))
        return;
    mCards[i].animTimer = 0;
    mCards[i].frame -= 1;
    if(mCards[i].frame == 0)
        mCards[i].state = 2;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov006_020f5e70, 0x020f5e70, size 0x4 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::CardWait(int /* card */)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov006_020f5e74, 0x020f5e74, size 0x98 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::CardFlipUp(int idx){
  char *base = (char *)this;
  unsigned short* cnt = (unsigned short*)(base + 0x51b6 + idx*0x18);
  unsigned char* st = (unsigned char*)(base + 0x51bd + idx*0x18);
  *cnt = *cnt + 1;
  if (*cnt < (data_ov006_0213d344[*st] & 0xff)) return;
  *cnt = 0;
  *st = *st + 1;
  if (*st > 4) {
    *st = 4;
    mCards[idx].state = 4;
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov006_020f5f0c, 0x020f5f0c, size 0x178 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::CardSelect(int idx)
{
    char *self = (char *)this;
    unsigned int e;
    unsigned int i;
    int ok;
    int n;
    int v, w;

    e = mSelectedCount;
    if (e >= 2) return;

    i = data_020a0e40;
    ok = 0;
    if (data_020a0de8[i * 4] != 0) {
        if (data_020a0de9[i * 4] != 0) {
            ok = 1;
        }
    }
    if (ok == 0) return;

    n = idx * 0x18;
    v = data_020a0dea[data_020a0e40 * 4] - (*(int*)(self + 0x51a8 + n) >> 12);
    w = data_020a0deb[data_020a0e40 * 4] - (*(int*)(self + 0x51ac + n) >> 12);

    if (v < -0x10) return;
    if (v > 0x10) return;
    if (w < -0x16) return;
    if (w > 0x16) return;

    *(u8*)(self + 0x53ee + e) = *(u8*)(self + 0x51b8 + n);
    *(u8*)(self + 0x53f0 + mSelectedCount) = (u8)idx;
    {
        u8 *pc = (u8*)(self + 0x5406);
        *pc = *pc + 1;
    }
    *(u8*)(self + 0x51bc + n) = 3;
    func_02012718(0x143, *(int*)(self + 0x51a8 + n));

    if (mInputSeen != 0) return;
    {
        u8 *pd = (u8*)(self + 0x540c);
        *pd = *pd + 1;
    }
    func_ov004_020ad79c(*(int*)(self + 0xa8), *(int*)(self + 0xb4));
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_020f6084, 0x020f6084, size 0x4 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::CardIdle(int /* card */)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov006_020f6088, 0x020f6088, size 0x1a8 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::CardMove(int i)
{
    int i2 = i * 2;
    u16* row = data_ov006_0213d338[mDifficulty];
    int dx, dy, a, b;
    a = row[i2];
    dx = a - (mCards[i].x >> 12);
    b = row[i2 + 1];
    dy = b - (mCards[i].y >> 12);

    mCards[i].angle = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);

    {
        s16 tv = data_02082214[((static_cast<u16>(mCards[i].angle) >> 4) << 1) + 1];
        mCards[i].x += (int)(((s64)tv * mCards[i].speed + 0x800) >> 0xc);
    }
    {
        s16 tv = data_02082214[(static_cast<u16>(mCards[i].angle) >> 4) << 1];
        mCards[i].y += (int)(((s64)tv * mCards[i].speed + 0x800) >> 0xc);
    }

    if (dx < -6)
        return;
    if (dx > 6)
        return;
    if (dy < -6)
        return;
    if (dy > 6)
        return;

    mCards[i].x = a << 12;
    mCards[i].y = b << 12;
    *&mCards[i].state = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov006_020f6230, 0x020f6230, size 0x16c */
/* -------------------------------------------------------------------------- */
#pragma push
#pragma opt_loop_invariants off
void dScMgMemory2_c::ResultFinish()
{
    char *p = (char *)this;
    int i;
    int best;
    int j;
    int n;
    int idx;
    int ok;
    unsigned short *q;

    UpdateCards();
    UpdateCursor();

    if (mCardTimer != 0) {
        q = (unsigned short *)(unsigned int)(p + 0x53e2);
        *q = *q - 1;
        return;
    }

    ok = 0;
    idx = data_020a0e40;
    if (data_020a0de8[idx * 4] != 0) {
        if (data_020a0de9[idx * 4] != 0) {
            ok = 1;
        }
    }
    if (ok == 0) {
        return;
    }

    func_02012790(0x62);

    if (*(int *)(p + 0xa8) == 0) {
        for (i = 0; i < data_ov006_0212e8e8[mDifficulty]; i++) {
            best = 0xff;
            j = 0;
            n = data_ov006_0212e8f4[mDifficulty];
            for (; j < data_ov006_0212e8f4[mDifficulty]; j++) {
                if (*(unsigned char *)(p + 0x51bb + (i * n + j) * 0x18) != 0) {
                    best = j;
                    best = best + i * n;
                }
            }
            if (best != 0xff) {
                mCards[best].state = 6;
            }
        }
        mMessageVisible = 0;
    }

    HideCursor();
    mState = 4;
}
#pragma pop

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov006_020f639c, 0x020f639c, size 0xec */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::ResultTurnCards()
{
    if (mCardTimer) {
        unsigned short* q = &mCardTimer;
        *q = *q - 1;
        return;
    }
    if (mPairsFound >= mTargetPairs) {
        mState = 4;
        mCardTimer = 0;
        return;
    }
    {
        int count = 0;
        int i = 0;
        for (; i < 0x14; i++) {
            if (mCards[i].active != 0) {
                if (mCards[i].state == 2) {
                    mCards[i].state = 3;
                    count++;
                }
            }
        }
        if (count <= 2) {
            Sound::PlayBank2_2D(0x145);
        } else {
            Sound::PlayBank2_2D(0x146);
        }
    }
    mSubstate = 3;
    ShowCursor();
    mCardTimer = 0x20;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov006_020f6488, 0x020f6488, size 0xb0 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::ResultReward() {
    char *c = (char *)this;
    if (*((unsigned short *)(c + 0x5300 + 0xE2)) != 0) {
        unsigned short *ptr = (unsigned short *)(((int)c) + 0x53E2);
        *ptr = *ptr - 1;
        if (*((unsigned short *)(c + 0x5300 + 0xE2)) != 0) {
            return;
        }
        if (*((unsigned char *)(c + 0x5000 + 0x405)) < *((unsigned short *)(c + 0x5300 + 0xEA))) {
            return;
        }
        func_ov004_020b56c8((char *)(5 * (5 - *((unsigned char *)(c + 0x5000 + 0x408)))));
        return;
    }
    if (data_ov004_020bf9e4 == 1) {
        *((unsigned short *)(c + 0x5300 + 0xE2)) = 0;
        *((int *)(c + 0x5000 + 0x3D8)) = 2;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov006_020f6538, 0x020f6538, size 0x140 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::ResultWait()
{
    char *c = (char *)this;
    struct B *p;

    if (*(unsigned short *)(c + 0x53e4) == 0)
        goto zero;

    *(unsigned short *)(((int)c + 0x53e4)) =
        *(unsigned short *)(((int)c + 0x53e4)) - 1;
    if (*(short *)(c + 0x53e4) > 0)
        return;

    *(unsigned short *)(c + 0x53e4) = 0;

    if (mPairsFound >= mTargetPairs) {
        if (mMisses == 0)
            func_ov004_020b67f8();
        func_ov004_020b0a54(4);
        p = (struct B *)data_ov004_020beb68;
        if (p != 0) {
            if (p->b4 < 0x270f)
                *(int *)(((int)p + 0xb4)) += 1;
            if (p->b4 > p->b8)
                p->b8 = p->b4;
        }
        func_ov004_020adb1c(data_ov004_020beb68 != 0
            ? ((struct B *)data_ov004_020beb68)->b4 : 0);
        func_ov006_020c0c80(c + 0x4f38);
        return;
    }

    func_ov004_020b0a54(5);
    p = (struct B *)data_ov004_020beb68;
    if (p != 0) {
        if (p->b4 > 0)
            *(int *)(((int)p + 0xb4)) -= 1;
    }
    func_ov006_020c0d68(c + 0x4f38);
    return;

zero:
    *(unsigned char *)(c + 0xc3) = 0;
    *(unsigned short *)(c + 0x53e2) = 0x10;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_020f6678, 0x020f6678, size 0x94 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::RoundReveal()
{
  char *c = (char *)this;
  char *q = c + 0x5300;
  char *h;
  if ((*((unsigned short *) (q + 0xe2))) != 0)
  {
    h = (char *) ((((int) c) + 0x53e2));
    *((unsigned short *) h) = (unsigned short) ((*((unsigned short *) h)) - 1);
    if ((*((short *) ((c + 0x5300) + 0xe2))) > 0)
    {
      return;
    }
    InitPlayers();
    return;
  }
  {
    long i;
    char *p;
    for (i = 0; i < 0x14; i++)
    {
      p = c;
      *((unsigned char *) (((i * 0x18) + p) + 0x51bc)) = 2;
    }

  }
  *((int *) (c + 0x53d8)) = 0;
  *((int *) (c + 0x53d4)) = 2;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov006_020f670c, 0x020f670c, size 0x94 */
/* -------------------------------------------------------------------------- */
void *dScMgMemory2_c::RoundWaitDeal(){
  char *c = (char *)this;
  int cnt = 0;
  int i = 0;
  char* p = c;
  for (; i < 0x14; ) {
    if (*(unsigned char*)(p + 0x51bb) != 0) {
      if (*(unsigned char*)(p + 0x51bc) != 1) {
        cnt++;
        break;
      }
    }
    i++;
    p += 0x18;
  }
  if (cnt != 0) return c;
  p = c;
  i = 0;
  {
    unsigned char v = 2;
    for (; i < 0x14; ) {
      *(unsigned char*)(p + 0x51bc) = v;
      i++;
      p += 0x18;
    }
  }
  {
    char* base = c + 0x5000;
    *(int*)(base + 0x3d8) = 0;
    *(int*)(base + 0x3d4) = 2;
    return base;
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov006_020f67a0, 0x020f67a0, size 0x90 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::RoundReadyCards(){
    char *c = (char *)this;
    int found = 0;
    int i;
    char *p;
    for(i = 0, p = c; i < 0x14; i++, p += 0x18){
        if(*(unsigned char*)(p + 0x51bb) != 0){
            if(*(unsigned char*)(p + 0x51bc) == 2)
                *(unsigned char*)(p + 0x51bc) = 1;
        }
    }
    for(i = 0, p = c; i < 0x14; i++, p += 0x18){
        if(*(unsigned char*)(p + 0x51bb) != 0){
            if(*(unsigned char*)(p + 0x51bc) != 1){ found++; break; }
        }
    }
    if(found == 0)
        mSubstate = 8;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov006_020f6830, 0x020f6830, size 0xd4 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::RoundHideCards()
{
  char *o = (char *)this;
  int count;
  int i;
  unsigned char *e;
  if ((*((unsigned short *) (o + 0x53ec))) != 0)
  {
    i = 0x53ec;
    *((unsigned short *) ((((int) o) + i))) -= 1;
    return;
  }
  e = (unsigned char *) o;
  for (i = 0; i < 0x14; i++)
  {
    if (e[0x51bb] != 0)
    {
      if (e[0x51bc] == 4)
      {
        e[0x51bc] = 5;
      }
    }
    e += 0x18;
  }

  count = 0;
  i = 0;
  for (e = (unsigned char *) o; i < 0x14; i++)
  {
    if (e[0x51bb] != 0)
    {
      unsigned char st = e[0x51bc];
      if ((st != 1) && (st != 5))
      {
        count++;
        break;
      }
    }
    e = e + 0x18;
  }

  if (count == 0)
  {
    *((int *) (o + 0x53d8)) = 7;
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov006_020f6904, 0x020f6904, size 0xfc */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::RoundShowCards()
{
    char *c = (char *)this;
    unsigned int off_bb;
    int cntA = 0;
    int cntB = 0;
    int i;
    int ff = 0xff;
    int three = 3;
    int off_ids = 0x53fd;
    int off_bc = 0x51bc;
    int stride = 0x18;

    for (i = 0; i < 8; i++) {
        unsigned char *slot = (unsigned char *)((c + i) + off_ids);
        unsigned char id = *slot;
        if (id == 0xff)
            continue;
        {
            int base = (int)c + id * stride;
            off_bb = 0x51bb;
            if (*(unsigned char *)(base + off_bb) != 0) {
                unsigned char *fp = (unsigned char *)(base + off_bc);
                if (*fp == 1) {
                    *fp = (unsigned char)three;
                    *slot = (unsigned char)ff;
                    cntB++;
                }
            } else {
                *slot = (unsigned char)ff;
            }
            cntA++;
        }
    }

    if (cntB == 1)
        Sound::PlayBank2_2D(0x148);
    else if (cntB == 2)
        Sound::PlayBank2_2D(0x145);
    else if (cntB >= 3)
        Sound::PlayBank2_2D(0x146);
    if (cntA != 0)
        return;
    mSubstate = 6;
    mPreviewTimer = mDifficulty * 20 + 0x50;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov006_020f6a00, 0x020f6a00, size 0x78 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::RoundDealFourth()
{
  char *self = (char *)this;
  int idx;

  if ((*((short *) ((self + 0x5300) + 0xe6))) < 4)
  {
    return;
  }
  idx = ((((unsigned short) (*((unsigned short *) ((self + 0x5300) + 0xea)))) << 1) - 1) - (*((short *) ((self + 0x5300) + 0xe8)));
  *((unsigned char *) ((self + (idx * 0x18)) + 0x51bb)) = 1;
  {
    short *sp = (short *) (((int) self + 0x53e8));
    *sp = (*sp) + 1;
  }
  self = self + 0x5000;
  if ((*((short *) ((self - 0x5000 + 0x5300) + 0xe8))) >= data_ov006_0212e948[*((unsigned char *) (self + 0x40a))])
  {
    *((int *) (self + 0x3d8)) = 5;
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov006_020f6a78, 0x020f6a78, size 0x88 */
/* -------------------------------------------------------------------------- */
/* func_ov006_020f6a78 — once at least 3 items are queued, mark slot
 * (total*2 - 1 - count) done (stride 0x18 array at 0x51a8), bump count;
 * when count reaches the per-mode threshold (data_ov006_0212e93c),
 * set state 5 (mode 1) or 4 (otherwise). */
void dScMgMemory2_c::RoundDealHard()
{
    Work* w = (Work*)this;
    u8 mode;
    if (w->ready < 3)
        return;
    w->slots[w->total * 2 - 1 - w->count].done = 1;
    (mDealCount)++;
    mode = w->mode;
    if (w->count < data_ov006_0212e93c[mode])
        return;
    if (mode == 1)
        w->state = 5;
    else
        w->state = 4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov006_020f6b00, 0x020f6b00, size 0x78 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::RoundDealNormal()
{
    char *self = (char *)this;
    int idx;
    char *new_var;
    if ((*((short *) ((self + 0x5300) + 0xe6))) < 2)
    {
        return;
    }
    idx = ((((unsigned short) (*((unsigned short *) ((self + 0x5300) + 0xea)))) << 1) - 1) - (*((short *) ((self + 0x5300) + 0xe8)));
    new_var = self + 0x5300;
    *((unsigned char *) ((self + (idx * 0x18)) + 0x51bb)) = 1;
    {
        short *p = (short *) (((long long)(self + 0x53e8)));
        *p = (*p) + 1;
    }
    if ((*((short *) (new_var + 0xe8))) >= data_ov006_0212e924[*((unsigned char *) (self + 0x540a))])
    {
        *((int *) (self + 0x53d8)) = 3;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- func_ov006_020f6b78, 0x020f6b78, size 0x78 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::RoundDealEasy()
{
  char *self = (char *)this;
  int idx;
  char *r3;
  if ((*((short *) ((self + 0x5300) + 0xe6))) < 1)
  {
    return;
  }
  idx = ((((unsigned short) (*((unsigned short *) ((self + 0x5300) + 0xea)))) << 1) - 1) - (*((short *) ((self + 0x5300) + 0xe8)));
  *((unsigned char *) ((self + (idx * 0x18)) + 0x51bb)) = 1;
  if ((self && self) && self)
  {
  }
  {
    short *p = (short *) (((int) (self + 0x53e8)));
    *p = (*p) + 1;
  }
  r3 = self + 0x5300;
  if ((*((short *) (r3 + 0xe8))) >= data_ov006_0212e918[*((unsigned char *) (self + 0x540a))])
  {
    *((int *) (self + 0x53d8)) = 2;
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov006_020f6bf0, 0x020f6bf0, size 0xa0 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::RoundStart(){
    char *c = (char *)this;
    mReadyCount = 0;
    mDealCount = 0;
    if (mDifficulty == 1)
        func_ov006_020c1604(c+0x4f38, 4, 3, (int)(c+0x53e6));
    else
        func_ov006_020c1604(c+0x4f38, 4, 4, (int)(c+0x53e6));
    mShared.ready = 1;
    mSubstate = 1;
    if (*(unsigned char*)(c+0xc4) == 0) {
        *(unsigned char*)(c+0xc3) = 1;
        *(unsigned char*)(c+0xc4) = 1;
        *(short*)(c+0xc0) = 0;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov006_020f6c90, 0x020f6c90, size 0x2f8 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::ShuffleCards()
{
    char *c = (char *)this;
    for (int i = 0; i < 11; i++)
        *(u8 *)(c + i + 0x53f2) = 0;
    for (int i = 0; i < 8; i++)
        *(u8 *)(c + i + 0x53fd) = 0xff;

    u8 flag = mDifficulty;
    if (flag == 0) {
        int i = 0;
        char *p = c;
        for (; i < 16; i++, p += 0x18) {
            u32 idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 8 >> 15);
            u8 *entry;
        check0:
            entry = (u8 *)(c + idx + 0x53f2);
            if (*entry < 2) {
                *(u8 *)(p + 0x51b8) = idx;
                (*entry)++;
            } else {
                idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 8 >> 15);
                goto check0;
            }
        }
        int j = 0;
        char *q = c;
        for (; j < 16; j++, q += 0x18) {
            *(int *)(q + 0x51a8) = 0x80000;
            *(int *)(q + 0x51ac) = -0x80000;
            *(int *)(q + 0x51b0) = 0x8000;
            *(u8 *)(q + 0x51ba) = 1;
            *(u8 *)(q + 0x51bc) = 0;
        }
        ChoosePreviewCards();
        return;
    } else if (flag == 1) {
        int i = 0;
        char *p = c;
        for (; i < 18; i++, p += 0x18) {
            u32 idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15);
            u8 *entry;
        check1:
            entry = (u8 *)(c + idx + 0x53f2);
            if (*entry < 2) {
                *(u8 *)(p + 0x51b8) = idx;
                (*entry)++;
            } else {
                idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 9 >> 15);
                goto check1;
            }
        }
        int j = 0;
        char *q = c;
        for (; j < 18; j++, q += 0x18) {
            *(int *)(q + 0x51a8) = 0x80000;
            *(int *)(q + 0x51ac) = -0x80000;
            *(int *)(q + 0x51b0) = 0x8000;
            *(u8 *)(q + 0x51ba) = 1;
            *(u8 *)(q + 0x51bc) = 0;
        }
        ChoosePreviewCards();
        return;
    } else {
        int i = 0;
        char *p = c;
        for (; i < 20; i++, p += 0x18) {
            u32 idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 10 >> 15);
            u8 *entry;
        check2:
            entry = (u8 *)(c + idx + 0x53f2);
            if (*entry < 2) {
                *(u8 *)(p + 0x51b8) = idx;
                (*entry)++;
            } else {
                idx = 1 + ((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 10 >> 15);
                goto check2;
            }
        }
        int j = 0;
        char *q = c;
        for (; j < 20; j++, q += 0x18) {
            *(int *)(q + 0x51a8) = 0x80000;
            *(int *)(q + 0x51ac) = -0x80000;
            *(int *)(q + 0x51b0) = 0x8000;
            *(u8 *)(q + 0x51ba) = 1;
            *(u8 *)(q + 0x51bc) = 0;
        }
        ChoosePreviewCards();
        return;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- func_ov006_020f6f88, 0x020f6f88, size 0xdc */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::ChoosePreviewCards()
{
    char *obj = (char *)this;
    int idx = mDifficulty;
    int n = data_ov006_0212e900[idx];
    int mul = data_ov006_0212e90c[idx];
    int i;
    int zero = 0;
    for (i = 0; i < n; i++) {
        int v;
        v = (mul * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 15;
        if (i != 0) {
            int found;
            int j;
            do {
                found = zero;
                for (j = zero; j < i; j++) {
                    if (v == *(unsigned char *)(obj + j + 0x5000 + 0x3fd)) {
                        found++;
                        break;
                    }
                }
                if (found == 0) break;
                v = (mul * (((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)) >> 15;
            } while (1);
        }
        *(unsigned char *)(obj + i + 0x5000 + 0x3fd) = (unsigned char)v;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- func_ov006_020f7064, 0x020f7064, size 0x12c */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::ResetGame()
{
    char *c = (char *)this;
    int i; char* p; int j;
    p = c;
    i = 0;
    do {
        *(int*)(p + 0x51a8) = 0;
        *(int*)(p + 0x51ac) = 0;
        *(int*)(p + 0x51b0) = 0;
        *(short*)(p + 0x51b4) = 0;
        *(short*)(p + 0x51b6) = 0;
        *(char*)(p + 0x51b8) = 0;
        *(char*)(p + 0x51b9) = 0;
        *(char*)(p + 0x51ba) = 0;
        *(char*)(p + 0x51bb) = 0;
        *(char*)(p + 0x51bc) = 0;
        *(char*)(p + 0x51bd) = 0;
        i++;
        *(char*)(p + 0x51be) = 0;
        p += 0x18;
    } while (i < 0x14);
    p = c;
    j = 0;
    do {
        *(int*)(p + 0x5388) = 0;
        *(int*)(p + 0x538c) = 0;
        *(int*)(p + 0x5390) = 0;
        *(short*)(p + 0x5396) = 0;
        *(short*)(p + 0x5394) = 0;
        *(char*)(p + 0x5398) = 0;
        *(char*)(p + 0x5399) = 0;
        j++;
        *(char*)(p + 0x539a) = 0;
        p += 0x14;
    } while (j < 3);
    p = c + 0x5000;
    *(char*)(p + 0x3d0) = 0;
    *(char*)(p + 0x3d2) = 0;
    *(char*)(p + 0x3d1) = 0;
    *(int*)(p + 0x3d4) = 0;
    *(int*)(p + 0x3d8) = 0;
    *(short*)(c + 0x53e0) = 0;
    *(short*)(c + 0x53e2) = 0;
    *(short*)(c + 0x53e4) = 0;
    *(char*)(p + 0x405) = 0;
    *(char*)(p + 0x406) = 0;
    *(char*)(p + 0x407) = 0;
    *(char*)(p + 0x408) = 0;
    i = 0;
    do {
        p = c + i;
        p += 0x5000;
        *(char*)(p + 0x3ee) = 0;
        i++;
        *(char*)(p + 0x3f0) = 0;
    } while (i < 2);
    j = 0;
    do {
        p = (c + j) + 0x5000;
        j++;
        *(char*)(p + 0x3f2) = 0;
    } while (j < 0xb);
    c += 0x5000;
    *(char*)(c + 0x40b) = 1;
    *(char*)(c + 0x40c) = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- func_ov006_020f7190, 0x020f7190, size 0x38 */
/* -------------------------------------------------------------------------- */
// Call a helper on self, then if the u16 counter at self+0x53e2 is nonzero, decrement
// it. The bl target is a relocation wildcard; the check folds to add+ldrh and the
// laundered RMW pool-loads the offset for the predicated tail.
void dScMgMemory2_c::StateExit()
{
    UpdateCards();
    if (mCardTimer)
        mCardTimer -= 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- func_ov006_020f71c8, 0x020f71c8, size 0x48 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::StateResult(){
  (this->*data_ov006_021423c0[mSubstate])();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 41 -- func_ov006_020f7210, 0x020f7210, size 0x24 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::StateJudge() {
    UpdateCards();
    JudgePair();
    CheckFinished();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- func_ov006_020f7234, 0x020f7234, size 0x4c */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::StatePlay() {
    int j = mSubstate;
    (this->*data_ov006_02142440[j])();
    UpdateCards();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 43 -- func_ov006_020f7280, 0x020f7280, size 0x40 */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::StateSetup(){
    char *c = (char *)this;
    SetupDifficulty();
    ShuffleCards();
    FreeGfxSlotsById(0x1d);
    func_ov006_020c1764(c + 0x4f38);
    mState = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 44 -- func_ov006_020f72c0, 0x020f72c0, size 0x4c */
/* -------------------------------------------------------------------------- */
void dScMgMemory2_c::SetupDifficulty() {
    char *c = (char *)this;
    mDifficulty = 0;
    mTargetPairs = 8;
    int x = *(int*)(c + 0xb4);
    if (x >= 0xa) {
        mDifficulty = 2;
        mTargetPairs = 0xa;
        return;
    }
    if (x >= 5) {
        mDifficulty = 1;
        mTargetPairs = 9;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 45 -- _ZN14dScMgMemory2_c15OnGroundPoundedEv, 0x020f730c, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMemory2_c15OnGroundPoundedEv
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
/* recovered: renamed to Class_Method, RTTI class fields named */
void dScMgMemory2_c::OnGroundPounded()
{

    func_ov004_020b63a0(mMaxMisses);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 46 -- _ZN14dScMgMemory2_c13OnTurnIntoEggEi, 0x020f7320, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMemory2_c13OnTurnIntoEggEi
/* recovered: renamed to Class_Method, RTTI class fields named */
int dScMgMemory2_c::OnTurnIntoEgg(int /* mode */)
{
    char *c = (char *)this;

    if (mState == 3 && mSubstate == 0) {
        if (func_ov006_020c1718((int*)(c + 0x4f38)) == 0) return 0;
        mSubstate = 1;
        FreeGfxSlotsById(0x1d);
    }
    return mState == 4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 47 -- _ZN14dScMgMemory2_c13OnYoshiTryEatEi, 0x020f7394, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMemory2_c13OnYoshiTryEatEi
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
/* recovered: renamed to Class_Method, RTTI class fields named */
void dScMgMemory2_c::OnYoshiTryEat(int /* arg */)
{

    char *o;
    int v;
    ResetGame();
    mState = 0;
    o = (char *)data_ov004_020beb68;
    v = 0;
    if (o != 0) v = *(int *)(o + 0xa8);
    if (v >= 5) v = 5;
    func_ov004_020b66d4();
    data_ov004_020bc7d4 = 1;
    mMaxMisses = (unsigned char)v;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 48 -- _ZN14dScMgMemory2_c6RenderEv, 0x020f73f4, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMemory2_c6RenderEv
/* dScMgMemory2_c::Render -- vtable slot 9.
 *
 * Attributed by the ROM's vtable: the third of the three slots where this class's
 * table differs from dScMgSingle3DBase_c's. Three draw passes over the card field
 * plus the shared dMeter_c, in the fixed order the ROM has.
 *
 * WAS A C99 FILE, so the three declarations move inside `extern "C"`; in C++ they
 * would mangle and resolve to nothing. */
s32 dScMgMemory2_c::Render()
{
    char *c = (char *)this;
    func_ov006_020c0aa8(c + 0x4660);
    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b6430();
    DrawMessage();
    DrawCursor();
    DrawCards();
    func_ov006_020c1804(c + 0x4f38);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 49 -- _ZN14dScMgMemory2_c8BehaviorEv, 0x020f7458, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMemory2_c8BehaviorEv
/* dScMgMemory2_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable: one of the three slots where this class's table
 * differs from dScMgSingle3DBase_c's. One dispatch through the pointer-to-member
 * table at data_ov006_021423e0, indexed by the state word at 0x53d4, then the two
 * per-frame calls every minigame in this family makes.
 *
 * __sinit_ov006_021314e4 constructs the five-entry member-function table in
 * StateSetup/StatePlay/StateJudge/StateResult/StateExit order. The receiver and
 * index here are the real scene and mState member. */
s32 dScMgMemory2_c::Behavior()
{
    (this->*data_ov006_021423e0[mState])();
    func_ov004_020b65e4();
    func_ov006_020c19d0((char *)this + 0x4f38);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 50 -- _ZN14dScMgMemory2_c13InitResourcesEv, 0x020f74b4, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN14dScMgMemory2_c13InitResourcesEv
/* dScMgMemory2_c::InitResources -- vtable slot 0.
 *
 * Attributed by the ROM's vtable: one of the three slots where this class's table
 * differs from dScMgSingle3DBase_c's. The old file's `recovered name:` comment
 * agreed, and here it is right.
 *
 * Loads the language-specific card art into both OBJ VRAM banks with the shared
 * palette, resets the class's own state, then arms the timer and the dMeter_c. The bail
 * on func_ov006_020c1a88 returning 0 is the only failure path, and it returns 0
 * from InitResources -- which is what the s32 return of fBase_c::InitResources
 * is for.
 *
 * func_ov006_0210a534 is dScMgSingle3DBase_c's slot 26, still unnamed at that
 * level, so it stays a mangled-free `extern "C"` call rather than being spelled as
 * an inherited method. */
s32 dScMgMemory2_c::InitResources()
{
    char *self = (char *)this;
    void *a;
    void *b;

    data_0209d45c = 0x11;
    func_ov006_0210a534();
    a = LoadFile(data_ov006_0213d370[GetGameLanguage()]);
    b = LoadFile(0xbb);
    DecompressLZ16(a, (void *)0x6400000);
    GX::LoadOBJPltt(b, 0, 0x100);
    DecompressLZ16(a, (void *)0x6600000);
    GXS::LoadOBJPltt(b, 0, 0x100);
    Deallocate(a);
    Deallocate(b);
    data_0209d454 = 0x18;
    ResetGame();
    func_ov006_020c0aa8(self + 0x4660);
    if (func_ov006_020c1a88(self + 0x4f38) == 0) return 0;
    *(int *)(self + 0xa8) = func_ov004_020ad8b8();
    *(int *)(self + 0xac) = *(int *)(self + 0xa8);
    *(int *)(self + 0xb4) = func_ov004_020ad878();
    func_ov004_020b66d4();
    data_ov004_020bc7d4 = 1;
    mMaxMisses = 5;
    func_ov004_020b04d0(0x20);
    func_ov004_020b682c();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 51 -- dScMgMemory2_c_classInit, 0x020f75d4, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol dScMgMemory2_c_classInit
/* Ordinary construction initializes the inherited particle tracker and
 * shared minigame state in the cartridge's observed order. */
extern "C" void *dScMgMemory2_c_classInit()
{
    return new dScMgMemory2_c;
}
