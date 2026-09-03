//cpp
/* Reconstructed translation unit.
 * ov006/dScMgMemory_c  (47 functions)
 *
 * The contiguous scene run and immediately adjacent unique MG_MEMORY factory
 * are one original object. The class identity is ROM-proven by RTTI; the state
 * and helper names attached to address-only symbols are descriptive coinage,
 * documented in the TU manifest and actor rename ledger.
 *
 * FUNCTION ORDER IS ROM-ASCENDING. This TU intentionally disables deferred
 * code generation so CodeWarrior emits each definition as encountered. That
 * makes #pragma push/pop genuinely scope the two legacy optimization settings
 * below to their own methods; a single-object compile proves all 47 sections
 * byte-exact and in cartridge order. Do not restore the generator's default
 * reverse ordering without re-proving every section.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020f3834  src/_ZN13dScMgMemory_cD1Ev.cpp
 *   [1] 0x020f3888  src/_ZN13dScMgMemory_cD0Ev.cpp
 *   [2] 0x020f38f0  src/func_ov006_020f38f0.c
 *   [3] 0x020f392c  src/func_ov006_020f392c.c
 *   [4] 0x020f3964  src/func_ov006_020f3964.c
 *   [5] 0x020f39c8  src/func_ov006_020f39c8.c
 *   [6] 0x020f39fc  src/func_ov006_020f39fc.c
 *   [7] 0x020f3a10  src/func_ov006_020f3a10.c
 *   [8] 0x020f3a14  src/func_ov006_020f3a14.c
 *   [9] 0x020f3ba0  src/func_ov006_020f3ba0.c
 *   [10] 0x020f3c0c  src/func_ov006_020f3c0c.c
 *   [11] 0x020f3c90  src/func_ov006_020f3c90.c
 *   [12] 0x020f3d34  src/func_ov006_020f3d34.cpp
 *   [13] 0x020f3e68  src/func_ov006_020f3e68.c
 *   [14] 0x020f3f10  src/func_ov006_020f3f10.cpp
 *   [15] 0x020f3f84  src/func_ov006_020f3f84.c
 *   [16] 0x020f411c  src/func_ov006_020f411c.c
 *   [17] 0x020f41ac  src/func_ov006_020f41ac.c
 *   [18] 0x020f41b0  src/func_ov006_020f41b0.c
 *   [19] 0x020f4248  src/func_ov006_020f4248.c
 *   [20] 0x020f43c0  src/func_ov006_020f43c0.c
 *   [21] 0x020f43c4  src/func_ov006_020f43c4.c
 *   [22] 0x020f456c  src/func_ov006_020f456c.c
 *   [23] 0x020f46ec  src/func_ov006_020f46ec.cpp
 *   [24] 0x020f47d8  src/func_ov006_020f47d8.c
 *   [25] 0x020f4888  src/func_ov006_020f4888.c
 *   [26] 0x020f49ac  src/func_ov006_020f49ac.c
 *   [27] 0x020f4a40  src/func_ov006_020f4a40.c
 *   [28] 0x020f4ad4  src/func_ov006_020f4ad4.c
 *   [29] 0x020f4b30  src/func_ov006_020f4b30.c
 *   [30] 0x020f4bbc  src/func_ov006_020f4bbc.c
 *   [31] 0x020f4c38  src/func_ov006_020f4c38.c
 *   [32] 0x020f4cd8  src/func_ov006_020f4cd8.c
 *   [33] 0x020f4f94  src/func_ov006_020f4f94.c
 *   [34] 0x020f50c0  src/func_ov006_020f50c0.c
 *   [35] 0x020f50f8  src/func_ov006_020f50f8.cpp
 *   [36] 0x020f5140  src/func_ov006_020f5140.c
 *   [37] 0x020f5164  src/func_ov006_020f5164.cpp
 *   [38] 0x020f51b0  src/func_ov006_020f51b0.c
 *   [39] 0x020f51f0  src/func_ov006_020f51f0.c
 *   [40] 0x020f523c  src/_ZN13dScMgMemory_c15OnGroundPoundedEv.cpp
 *   [41] 0x020f5250  src/_ZN13dScMgMemory_c13OnTurnIntoEggEi.cpp
 *   [42] 0x020f52c4  src/_ZN13dScMgMemory_c13OnYoshiTryEatEi.cpp
 *   [43] 0x020f5324  src/_ZN13dScMgMemory_c6RenderEv.cpp
 *   [44] 0x020f5388  src/_ZN13dScMgMemory_c8BehaviorEv.cpp
 *   [45] 0x020f53e4  src/_ZN13dScMgMemory_c13InitResourcesEv.cpp
 *   [46] 0x020f5504  src/MgMemoryMatch_Spawn.c
 */

/* CODEWARRIOR CODEGEN BOUNDARY. With the compiler's default deferred emission,
 * a later option state wins even across #pragma push/pop. `defer_codegen off`
 * emits definitions eagerly, allowing DrawCards' opt_propagation setting and
 * CardFlyAway's opt_lifetimes setting to be scoped to those functions. This is
 * measured compiler behavior: removing any part changes ROM bytes.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "dScMgMemory_c.h"
#include "common.h"
#include "types.h"
#include "decl_common.h"



/* Local implementation views retained where replacing an exact legacy view
 * with the public member type measurably changes CodeWarrior register or
 * scheduling choices. The owning scene layout itself is declared in the real
 * dScMgMemory_c header. */
/* shadow namespace 'Sound' */
namespace Sound { void PlayBank2_2D(unsigned int); }

typedef void (dScMgMemory_c::*dScMgMemory_cState)();
typedef void (dScMgMemory_c::*dScMgMemory_cCardState)(int);
extern "C" dScMgMemory_cState data_ov006_021422bc[];
extern "C" dScMgMemory_cState data_ov006_021422dc[];
extern dScMgMemory_cState data_ov006_02142304[];
extern dScMgMemory_cCardState data_ov006_02142334[];

/* shadow typedef 'Ent' */
typedef struct Ent
{
    int x;
    unsigned char pad0[0xe];
    unsigned char f12;
    unsigned char f13;
    unsigned char f14;
    unsigned char pad1;
    unsigned char f16;
    unsigned char pad2;
} Ent;

/* shadow typedef 'Big' */
typedef struct Big
{
    unsigned char pad0[0x51a8];
    Ent arr[12];
    unsigned char pad1[0x74];
    unsigned char mode;
} Big;

/* shadow namespace 'GX' */
namespace GX { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

/* shadow namespace 'GXS' */
namespace GXS { void LoadOBJPltt(void const *, unsigned int, unsigned int); }

extern "C" {
extern void func_ov004_020b1e34(void* c, int a, int b, int d);
extern void func_ov004_020b0d8c(void *c, int arg1, int arg2);
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern short data_02082214[];
void func_02012790(int a);
void func_ov004_020b5dd4(void);
extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern u16 data_ov006_0213d168[];
extern void *data_ov006_0214236c[];
extern unsigned short data_ov006_0213d0a8[];
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern void func_ov004_020ad79c(int a, int b);
extern s16 data_02082214[];
extern u16* data_ov006_0213d09c[];
extern int func_ov004_020b56c8(int);
extern short data_ov004_020bf9e4;
extern void func_ov004_020b67f8(void);
extern void func_ov004_020b0a54(int c);
extern void func_ov004_020adb1c(int self);
extern void func_ov006_020c0c80(void *c);
extern void func_ov006_020c1604(char *c, int unused, short count, int ready);
extern int data_0209d4b8;
extern int RandomIntInternal(int *seed);
extern void FreeGfxSlotsById(int x);
void func_ov006_020c0aa8(char *p);
void func_ov004_020b1bc8(char *c, int a, int b, int d);
void func_ov006_020c1804(char *p);
void func_ov004_020b65e4(void);
int func_ov006_020c19d0(void *p);
void func_ov006_0210a534(void);
s32 GetGameLanguage(void);
void *LoadFile(int handle);
void DecompressLZ16(void *src, void *dst);
void Deallocate(void *ptr);
int func_ov006_020c1a88(void *c);
int func_ov004_020ad8b8(void);
int func_ov004_020ad878(void);
void func_ov004_020b66d4(void);
int func_ov006_020c1718(int *p);
void func_ov004_020b04d0(int v);
void func_ov004_020b682c(void);
extern u8 data_0209d45c;
extern u8 data_0209d454;
void *_ZN7fBase_cnwEj(unsigned int size);
void _ZN11dScMgBase_cC2Ev(void *scene);
void _ZN8Particle10SysTrackerC1Ev(void *tracker);
void func_ov006_020c1d80(void *sharedState);
extern void *_ZTV19dScMgSingle3DBase_c[];
}

/* -------------------------------------------------------------------------- */
#pragma defer_codegen off

/* ROM ordinal 0 -- _ZN13dScMgMemory_cD1Ev, 0x020f3834, size 0x54 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScMgMemory_cD1Ev
/* recovered: real C++ destructor. The one explicit call reproduces the
   ROM's own recovered body exactly: destroy the shared 0x270-byte table at
   0x4f38, addressed by raw offset from `this` because the header keeps it
   opaque. Everything after -- own vtable store, mSysTracker destruction,
   chain to ~dScMgBase_c() -- is the compiler's own inlining of
   dScMgSingle3DBase_c's now-inline destructor (see
   include/dScMgSingle3DBase_c.h's own note).

   The pre-migration source destroyed mSysTracker through a LOCAL
   `struct SysTracker { ~SysTracker(); char pad[4]; };` shadow. That
   mangles to _ZN10SysTrackerD1Ev, a symbol that exists nowhere in the ROM
   -- the real one is namespaced, _ZN8Particle10SysTrackerD1Ev. The bytes
   matched anyway, because build_pin and fdiff wildcard relocated words, so
   the file byte-matched while eligible.py quietly refused to enroll it and
   dsd served the ROM's own bytes. Inheriting the base's real member is what
   retires that phantom reference. */
/* D1 is emitted with D0 from the single destructor definition above. */
/* ROM ordinal 1 -- _ZN13dScMgMemory_cD0Ev, 0x020f3888, size 0x68 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScMgMemory_cD0Ev
/* recovered: real C++ destructor. The one explicit call reproduces the
   ROM's own recovered body exactly: destroy the shared 0x270-byte table at
   0x4f38, addressed by raw offset from `this` because the header keeps it
   opaque. Everything after -- own vtable store, mSysTracker destruction,
   chain to ~dScMgBase_c() -- is the compiler's own inlining of
   dScMgSingle3DBase_c's now-inline destructor (see
   include/dScMgSingle3DBase_c.h's own note).

   The pre-migration source destroyed mSysTracker through a LOCAL
   `struct SysTracker { ~SysTracker(); char pad[4]; };` shadow. That
   mangles to _ZN10SysTrackerD1Ev, a symbol that exists nowhere in the ROM
   -- the real one is namespaced, _ZN8Particle10SysTrackerD1Ev. The bytes
   matched anyway, because build_pin and fdiff wildcard relocated words, so
   the file byte-matched while eligible.py quietly refused to enroll it and
   dsd served the ROM's own bytes. Inheriting the base's real member is what
   retires that phantom reference. */
dScMgMemory_c::~dScMgMemory_c()
{
    /* mShared is destroyed automatically before the inherited scene. */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov006_020f38f0, 0x020f38f0, size 0x3c */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::DrawMessage(){
  if (mMessageVisible == 0) return;
  func_ov004_020b1e34(this, 0xe0, 0x14, 1);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov006_020f392c, 0x020f392c, size 0x38 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::DrawCursor() {
    if (mCursor.enabled == 0) return;
    func_ov004_020b0d8c(this, 0xe0, 0xa0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- func_ov006_020f3964, 0x020f3964, size 0x64 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::UpdateCursor()
{
    if (mCursor.visible == 0)
        return;

    {
        unsigned short* e = (unsigned short*)&mCursor.angle;
        *e = *e + 1;
        if (*e < 0x14)
            return;
        *e = 0;
    }
    {
        unsigned char* p = &mCursor.frame;
        *p = *p + 1;
        *p = *p & 1;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- func_ov006_020f39c8, 0x020f39c8, size 0x34 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::ShowCursor()
{
    mCursor.visible = 1;
    mCursor.enabled = 1;
    mCursor.x = 524288;
    mCursor.y = 688128;
    mCursor.angle = 0;
    mCursor.frame = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- func_ov006_020f39fc, 0x020f39fc, size 0x14 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::HideCursor()
{
    mCursor.visible = 0;
    mCursor.enabled = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- func_ov006_020f3a10, 0x020f3a10, size 0x4 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::PlayerWait(int /* player */)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- func_ov006_020f3a14, 0x020f3a14, size 0x18c */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::PlayerMove(int i)
{
    char *c = (char*)this;
    int off = i * 0x14;
    int tx = i * 16 + 12;
    int dx, dy, idx;

    dx = tx - (*(int*)(c + 0x52c8 + off) >> 12);
    dy = -44 - (*(int*)(c + 0x52cc + off) >> 12);

    *(short*)(c + 0x52d4 + off) = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);

    *(int*)(c + 0x52d0 + off) += 0x200;

    idx = *(unsigned short*)(c + 0x52d4 + off) >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = *(int*)(c + 0x52d0 + off);
        *(int*)(c + 0x52c8 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
    }

    idx = *(unsigned short*)(c + 0x52d4 + off) >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = *(int*)(c + 0x52d0 + off);
        *(int*)(c + 0x52cc + off) += (int)(((long long)tv * spd + 0x800) >> 12);
    }

    dx = tx - (*(int*)(c + 0x52c8 + off) >> 12);
    dy = -44 - (*(int*)(c + 0x52cc + off) >> 12);

    if (dx < -3) return;
    if (dx > 3) return;
    if (dy < -3) return;
    if (dy > 3) return;

    *(int*)(c + 0x52c8 + off) = tx << 12;
    *(int*)(c + 0x52cc + off) = -0x2c000;
    *(unsigned char*)(c + off + 0x5000 + 0x2da) = 2;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- func_ov006_020f3ba0, 0x020f3ba0, size 0x6c */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::PlayerDrop(int i){
    char *c = (char*)this;
    short *e = (short*)(c + 0x52d6 + i*0x14);
    if(*(unsigned short*)e != 0){
        *e = *(unsigned short*)e - 1;
        if(*e < 0) *e = 0;
        return;
    }
    *(int*)(c+i*0x14+0x5000+0x2c8)=0xc000;
    *(int*)(c+i*0x14+0x5000+0x2cc)=0xc000;
    *(unsigned char*)(c+i*0x14+0x5000+0x2d9)=1;
    *(unsigned char*)(c+i*0x14+0x5000+0x2da)=1;
    *(int*)(c+i*0x14+0x5000+0x2d0)=0xc00;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 10 -- func_ov006_020f3c0c, 0x020f3c0c, size 0x84 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::InitPlayers()
{
    char *self = (char*)this;
    int n;
    int i;
    int angle;
    char *entry;

    if (data_ov004_020beb68 != 0)
        n = *(int *)((char *)data_ov004_020beb68 + 0xa8);
    else
        n = 0;
    if (n >= 3)
        n = 3;

    entry = self;
    i = 0;
    *(unsigned char *)(self + 0x533b) = (unsigned char)n;
    if (n <= 0)
        return;

    angle = 0;
    do {
        *(unsigned char *)(entry + 0x52d8) = 1;
        *(unsigned char *)(entry + 0x52da) = 0;
        *(short *)(entry + 0x52d6) = angle;
        i++;
        entry += 0x14;
        angle += 0x10;
    } while (i < n);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 11 -- func_ov006_020f3c90, 0x020f3c90, size 0xa4 */
/* -------------------------------------------------------------------------- */
inline int *inline_fn(char *arg0)
{
  return (int *) arg0;
}

void dScMgMemory_c::CheckFinished()
{
  char *c = (char*)this;
  if (mPairsFound >= mTargetPairs)
  {
    mResultTimer = 0x40;
    mState = 3;
    mSubstate = 0;
    /* This equivalent view is load-order-sensitive in mwccarm 1.2: direct
       members swap r1/r2 for the multiply-accumulate. */
    func_ov004_020ad79c((*inline_fn(c + 0xa8)) + ((3 - (*((unsigned char *) (c + 0x533a)))) * 6), mHudScore + 1);
  }
  if (mMisses >= mMaxMisses)
  {
    mResultTimer = 0x40;
    mState = 3;
    mSubstate = 0;
    func_ov004_020ad79c(unk_0a8, mHudScore - 1);
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 12 -- func_ov006_020f3d34, 0x020f3d34, size 0x134 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::JudgePair()
{
    char *self = (char*)this;
    int a, b;
    unsigned char *ea, *eb;
    if (*(unsigned char *)(self + 0x5338) < 2) return;
    a = *(unsigned char *)(self + 0x532e);
    b = *(unsigned char *)(self + 0x532f);
    ea = (unsigned char *)(self + 0x51bc + a * 0x18);
    if (*ea != 4) return;
    eb = (unsigned char *)(self + 0x51bc + b * 0x18);
    if (*eb != 4) return;
    if (*(unsigned char *)(self + 0x51b8 + a * 0x18) == *(unsigned char *)(self + 0x51b8 + b * 0x18)) {
        *(unsigned char *)(self + 0x51bb + a * 0x18) = 0;
        *(unsigned char *)(self + 0x51ba + a * 0x18) = 0;
        *(unsigned char *)(self + 0x51bb + b * 0x18) = 0;
        *(unsigned char *)(self + 0x51ba + b * 0x18) = 0;
        func_02012790(0x26);
        Sound::PlayBank2_2D(0x13d);
        (*(unsigned char *)(((int)self + 0x5337)))++;
        *(unsigned char *)(self + 0x5338) = 0;
    } else {
        func_02012790(0xe);
        (*(unsigned char *)(((int)self + 0x533a)))++;
        func_ov004_020b5dd4();
        Sound::PlayBank2_2D(0x13e);
        if (*(unsigned char *)(self + 0x533a) < *(unsigned char *)(self + 0x533b)) {
            *ea = 5;
            *eb = 5;
            *(unsigned char *)(self + 0x5338) = 0;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 13 -- func_ov006_020f3e68, 0x020f3e68, size 0xa8 */
/* -------------------------------------------------------------------------- */
// 6f: keep constant live / flip coloring
#pragma push
#pragma opt_propagation off
void dScMgMemory_c::DrawCards()
{
    char *a0 = (char*)this;
    int i;
    int k, flag;
    char *p;

    /* p walks 12 HUD-sprite records at a0+0x51a8, stride 0x18.
       data_ov006_0213d168 is indexed as [id][5]. */
    p = a0;
    i = 0;
    do {
        if (*(u8 *)(p + 0x51ba) != 0) {
            flag = 0;
            if (mState != 2)
                flag = 1;
            k = data_ov006_0213d168[*(u8 *)(p + 0x51b8) * 5
                                    + *(u8 *)(p + 0x51bd)];
            Hud_RenderSprite(data_ov006_0214236c[k],
                             *(int *)(p + 0x51a8) >> 12,
                             *(int *)(p + 0x51ac) >> 12, -1, flag);
        }
        i += 1;
        p += 0x18;
    } while (i < 12);
}

/* -------------------------------------------------------------------------- */
#pragma pop

/* ROM ordinal 14 -- func_ov006_020f3f10, 0x020f3f10, size 0x74 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::UpdateCards() {
    dScMgMemory_c* self = this;
    int i;
    char* card = (char*)self;
    for (i = 0; i < 0xc; i++) {
        if (*(unsigned char*)(card + 0x51bb)) {
            (self->*data_ov006_02142334[*(unsigned char*)(card + 0x51bc)])(i);
        }
        card += 0x18;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 15 -- func_ov006_020f3f84, 0x020f3f84, size 0x198 */
/* -------------------------------------------------------------------------- */
#pragma push
#pragma opt_lifetimes off
void dScMgMemory_c::CardFlyAway(int i)
{
    Big *p = (Big*)this;
    int j;
    int n;
    unsigned char *pf;
    int *px;

    px = &p->arr[i].x;
    pf = &p->arr[i].f16;
    *px -= 0x10000;
    if (*pf == 0)
    {
        if (p->mode == 1)
        {
            if (i == 6 || i == 0xb)
            {
                *pf = 1;
            }
            n = (i >= 7) ? 0xc - i : 7 - i;
            for (j = 1; j < n; j++)
            {
                if (p->arr[i + j].f13 != 0)
                {
                    if ((((*(int *)((char *)(&((Ent *)((void *)p))[i]) + 0x51a8)) - p->arr[i + j].x) >> 12) <= 4)
                    {
                        *pf = 1;
                        p->arr[i + j].f14 = 6;
                        break;
                    }
                }
            }
        }
        else
        {
            if ((i & 3) == 3)
            {
                *pf = 1;
            }
            j = 4 - (i & 3);
            for (n = 1; n < j; n++)
            {
                if (p->arr[i + n].f13 != 0)
                {
                    if ((((*(int *)((char *)(&((Ent *)((void *)p))[i]) + 0x51a8)) - p->arr[i + n].x) >> 12) <= 4)
                    {
                        *pf = 1;
                        p->arr[i + n].f14 = 6;
                        break;
                    }
                }
            }
        }
    }
    if ((*px >> 12) > -0x18)
    {
        return;
    }
    p->arr[i].f13 = 0;
    p->arr[i].f12 = 0;
}

/* -------------------------------------------------------------------------- */
#pragma pop

/* ROM ordinal 16 -- func_ov006_020f411c, 0x020f411c, size 0x90 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::CardFlipDown(int i){
    char *c = (char*)this;
    int o = i * 0x18;
    unsigned short cnt;
    *(unsigned short*)(c + 0x51b6 + o) += 1;
    cnt = *(unsigned short*)(c + 0x51b6 + o);
    if(cnt < (data_ov006_0213d0a8[*(unsigned char*)(c + 0x5000 + o + 0x1bd)] & 0xff))
        return;
    *(unsigned short*)(c + 0x51b6 + o) = 0;
    *(unsigned char*)(c + 0x51bd + o) -= 1;
    if(*(unsigned char*)(c + 0x51bd + o) == 0)
        *(unsigned char*)(c + 0x5000 + o + 0x1bc) = 2;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 17 -- func_ov006_020f41ac, 0x020f41ac, size 0x4 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::CardWait(int /* card */)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 18 -- func_ov006_020f41b0, 0x020f41b0, size 0x98 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::CardFlipUp(int idx){
  char* base = (char*)this;
  unsigned short* cnt = (unsigned short*)(base + 0x51b6 + idx*0x18);
  unsigned char* st = (unsigned char*)(base + 0x51bd + idx*0x18);
  *cnt = *cnt + 1;
  if (*cnt < (data_ov006_0213d0a8[*st] & 0xff)) return;
  *cnt = 0;
  *st = *st + 1;
  if (*st > 4) {
    *st = 4;
    *(unsigned char*)(base + idx*0x18 + 0x5000 + 0x1bc) = 4;
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 19 -- func_ov006_020f4248, 0x020f4248, size 0x178 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::CardSelect(int idx)
{
  char *self = (char*)this;
  unsigned int s = mSelectedCount;
  unsigned int i;
  int ok;
  char *new_var;
  int n;
  int v;
  int w;
  if (s >= 2)
  {
    return;
  }
  i = data_020a0e40;
  ok = 0;
  if (data_020a0de8[data_020a0e40 * 4] != 0)
  {
    if (data_020a0de9[data_020a0e40 * 4] != 0)
    {
      ok = 1;
    }
  }
  if (ok == 0)
  {
    return;
  }
  n = idx * 0x18;
  v = data_020a0dea[i * 4] - ((*((int *) ((self + 0x51a8) + n))) >> 12);
  new_var = self + 0x51ac;
  w = data_020a0deb[i * 4] - ((*((int *) (new_var + n))) >> 12);
  if (v < (-0x10))
  {
    return;
  }
  if (v > 0x10)
  {
    return;
  }
  if (w < (-0x16))
  {
    return;
  }
  if (w > 0x16)
  {
    return;
  }
  mSelectedValues[s] = *((u8 *) ((self + 0x51b8) + n));
  mSelectedCards[mSelectedCount] = (u8) idx;
  {
    u8 *pc = &mSelectedCount;
    *pc = (*pc) + 1;
  }
  *((u8 *) ((self + 0x51bc) + n)) = 3;
  func_02012718(0x143, *((int *) ((self + 0x51a8) + n)));
  if (mInputSeen != 0)
  {
    return;
  }
  {
    u8 *pd = &mInputSeen;
    *pd = (*pd) + 1;
  }
  func_ov004_020ad79c(unk_0a8, mHudScore);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_020f43c0, 0x020f43c0, size 0x4 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::CardIdle(int /* card */)
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov006_020f43c4, 0x020f43c4, size 0x1a8 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::CardMove(int i)
{
    char* self = (char*)this;
    int i2 = i * 2;
    u16* row = data_ov006_0213d09c[mDifficulty];
    int i18 = i * 0x18;
    int dx, dy, a, b;
    a = row[i2];
    dx = a - (*(int*)(self + 0x51a8 + i18) >> 12);
    b = row[i2 + 1];
    dy = b - (*(int*)(self + 0x51ac + i18) >> 12);

    *(u16*)(self + 0x51b4 + i18) = (u16)_ZN4cstd5atan2E5Fix12IiES1_(dy, dx);

    {
        s16 tv = data_02082214[((*(u16*)(self + 0x51b4 + i18) >> 4) << 1) + 1];
        *(int*)(self + 0x51a8 + i18) += (int)(((s64)tv * *(int*)(self + 0x51b0 + i18) + 0x800) >> 0xc);
    }
    {
        s16 tv = data_02082214[(*(u16*)(self + 0x51b4 + i18) >> 4) << 1];
        *(int*)(self + 0x51ac + i18) += (int)(((s64)tv * *(int*)(self + 0x51b0 + i18) + 0x800) >> 0xc);
    }

    if (dx < -6)
        return;
    if (dx > 6)
        return;
    if (dy < -6)
        return;
    if (dy > 6)
        return;

    *(int*)(self + 0x51a8 + i18) = a << 12;
    *(int*)(self + 0x51ac + i18) = b << 12;
    *(u8*)(self + i18 + 0x51bc) = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov006_020f456c, 0x020f456c, size 0x180 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::ResultFinish()
{
    s32 i;
    s32 j;
    s32 k;
    s32 j2;
    s32 k2;
    s32 ok;
    u16 *p;

    UpdateCards();
    UpdateCursor();

    if (mCardTimer != 0)
    {
        p = &mCardTimer;
        *p = (u16)(*p - 1);
        return;
    }

    i = data_020a0e40;
    ok = 0;
    if (data_020a0de8[i * 4] != 0)
    {
        if (data_020a0de9[i * 4] != 0)
        {
            ok = 1;
        }
    }
    if (ok == 0)
    {
        return;
    }

    func_02012790(0x62);

    if (unk_0a8 == 0)
    {
        if (mDifficulty == 1)
        {
            for (j = 0; j < 2; j++)
            {
                for (k = 0; k < 5; k++)
                {
                    if (mCards[j * 5 + k + 2].active != 0)
                    {
                        mCards[j * 5 + k + 2].state = 6;
                        break;
                    }
                }
            }
        }
        else
        {
            for (j2 = 0; j2 < 3; j2++)
            {
                for (k2 = 0; k2 < 4; k2++)
                {
                    if (mCards[j2 * 4 + k2].active != 0)
                    {
                        mCards[j2 * 4 + k2].state = 6;
                        break;
                    }
                }
            }
        }
        mMessageVisible = 0;
    }

    HideCursor();
    mState = 4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov006_020f46ec, 0x020f46ec, size 0xec */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::ResultTurnCards()
{
    char *c = (char*)this;
    int cnt;
    int i;
    char *p;
    if (mCardTimer != 0) {
        unsigned short *q = &mCardTimer;
        *q = *q - 1;
        return;
    }
    if (mPairsFound >= mTargetPairs) {
        mState = 4;
        mCardTimer = 0;
        return;
    }
    cnt = 0;
    i = 0;
    p = c;
    for (; i < 0xc; i++) {
        if (*(unsigned char *)(p + 0x51bb) != 0) {
            if (*(unsigned char *)(p + 0x51bc) == 2) {
                *(unsigned char *)(p + 0x51bc) = 3;
                cnt++;
            }
        }
        p += 0x18;
    }
    if (cnt <= 2)
        Sound::PlayBank2_2D(0x145);
    else
        Sound::PlayBank2_2D(0x146);
    mSubstate = 3;
    ShowCursor();
    mCardTimer = 0x20;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov006_020f47d8, 0x020f47d8, size 0xb0 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::ResultReward()
{
  if (mCardTimer != 0)
  {
    unsigned short *ptr = &mCardTimer;
    *ptr = (*ptr) - 1;
    if (mCardTimer != 0)
    {
      return;
    }
    {
      unsigned char b = mPairsFound;
      if (b < mTargetPairs)
      {
        return;
      }
    }
    {
      int val = 6;
      val = (val * (3 - mMisses)) & 0xFFFFFFFFFFFFFFFFu;
      func_ov004_020b56c8(val);
      return;
    }
  }
  if (data_ov004_020bf9e4 == 1)
  {
    mCardTimer = 0;
    mSubstate = 2;
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov006_020f4888, 0x020f4888, size 0x124 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::ResultWait()
{
    if (mResultTimer != 0) {
        mResultTimer -= 1;
        if ((short)mResultTimer <= 0) {
            mResultTimer = 0;
            if (mPairsFound >= mTargetPairs) {
                if (mMisses == 0)
                    func_ov004_020b67f8();
                func_ov004_020b0a54(4);
                if (mHudScore < 0x270f) {
                    int *c = &mHudScore;
                    *c += 1;
                }
                if (mHudScore > unk_0b8)
                    unk_0b8 = mHudScore;
                func_ov004_020adb1c(data_ov004_020beb68 ? *(int *)((char *)data_ov004_020beb68 + 0xb4) : 0);
                func_ov006_020c0c80(&mShared);
            } else {
                func_ov004_020b0a54(5);
                if (mHudScore > 0) {
                    int *c = &mHudScore;
                    *c -= 1;
                }
                func_ov006_020c0d68(&mShared);
            }
        }
    } else {
        mPromptEnabled = 0;
        mCardTimer = 0x10;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_020f49ac, 0x020f49ac, size 0x94 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::RoundReveal()
{
    char *c = (char*)this;
    int i;
    char *e;
    unsigned short *t;
    if (mCardTimer != 0) {
        t = &mCardTimer;
        *t = *t - 1;
        if ((short)mCardTimer > 0)
            return;
        InitPlayers();
        return;
    }
    e = c;
    for (i = 0; i < 0xc; i++) {
        *(unsigned char *)(e + 0x5000 + 0x1bc) = 2;
        e += 0x18;
    }
    mSubstate = 0;
    mState = 2;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov006_020f4a40, 0x020f4a40, size 0x94 */
/* -------------------------------------------------------------------------- */
void* dScMgMemory_c::RoundWaitDeal(){
  char* c = (char*)this;
  int cnt = 0;
  int i = 0;
  char* p = c;
  for (; i < 0xc; ) {
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
    for (; i < 0xc; ) {
      *(unsigned char*)(p + 0x51bc) = v;
      i++;
      p += 0x18;
    }
  }
  {
    char* base = c + 0x5000;
    mSubstate = 0;
    mState = 2;
    return base;
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov006_020f4ad4, 0x020f4ad4, size 0x5c */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::RoundDealHard(){
  short n = mReadyCount;
  short k;
  short* q;
  if (n < 3) return;
  k = mDealCount;
  mCards[0xb - k].active = 1;
  q = &mDealCount;
  *q = *q + 1;
  if (mDealCount >= 0xc) mSubstate = 4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov006_020f4b30, 0x020f4b30, size 0x8c */
/* -------------------------------------------------------------------------- */
/* func_ov006_020f4b30 — once at least 2 items are ready, mark slot
 * (11 - count) done (stride 0x18 array at 0x51a8), bump count; then per
 * mode: mode 1 -> state 4 at count >= 10, else state 3/4 at count >= 8. */
void dScMgMemory_c::RoundDealNormal()
{
    u8 mode;
    if (mReadyCount < 2)
        return;
    mCards[0xb - mDealCount].active = 1;
    mDealCount++;
    mode = mDifficulty;
    if (mode == 1) {
        if (mDealCount >= 10)
            mSubstate = 4;
        return;
    }
    if (mDealCount < 8)
        return;
    if (mode != 0)
        mSubstate = 3;
    else
        mSubstate = 4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- func_ov006_020f4bbc, 0x020f4bbc, size 0x7c */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::RoundDealEasy(){
  short n = mReadyCount;
  short k;
  short* q;
  if (n < 1) return;
  k = mDealCount;
  mCards[0xb - k].active = 1;
  q = &mDealCount;
  *q = *q + 1;
  if (mDifficulty == 1) {
    if (mDealCount >= 5) mSubstate = 2;
  } else {
    if (mDealCount >= 4) mSubstate = 2;
  }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- func_ov006_020f4c38, 0x020f4c38, size 0xa0 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::RoundStart(){
    mReadyCount = 0;
    mDealCount = 0;
    if (mDifficulty >= 2)
        func_ov006_020c1604((char *)&mShared, 4, 3, (int)&mReadyCount);
    else
        func_ov006_020c1604((char *)&mShared, 4, 2, (int)&mReadyCount);
    mShared.ready = 1;
    mSubstate = 1;
    if (mPromptBlinkCount == 0) {
        mPromptEnabled = 1;
        mPromptBlinkCount = 1;
        mPromptBlinkTimer = 0;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov006_020f4cd8, 0x020f4cd8, size 0x2bc */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::ShuffleCards()
{
    char *c = (char*)this;
    int slot;
    int k;
    u8 *q;
    int i;
    char *p;

    for (i = 0; i < 7; i++)
        *(u8 *)(c + i + 0x5330) = 0;

    if (mDifficulty == 0) {
        p = c + 0x60;
        for (k = 4; k < 12; k++) {
            slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4 >> 15) + 1;
            for (;;) {
                q = (u8 *)(((int)c + slot + 0x5330));
                if (*q < 2) {
                    *(u8 *)(p + 0x51b8) = slot;
                    *q += 1;
                    break;
                }
                slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 4 >> 15) + 1;
            }
            p += 0x18;
        }
        p = c + 0x60;
        for (i = 4; i < 12; i++) {
            *(int *)(p + 0x51a8) = 0x80000;
            *(int *)(p + 0x51ac) = -0x80000;
            *(int *)(p + 0x51b0) = 0x8000;
            *(u8 *)(p + 0x51ba) = 1;
            *(u8 *)(p + 0x51bc) = 0;
            p += 0x18;
        }
    } else if (mDifficulty == 1) {
        p = c + 0x30;
        for (i = 2; i < 12; i++) {
            slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15) + 1;
            for (;;) {
                q = (u8 *)(((int)c + slot + 0x5330));
                if (*q < 2) {
                    *(u8 *)(p + 0x51b8) = slot;
                    *q += 1;
                    break;
                }
                slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 5 >> 15) + 1;
            }
            p += 0x18;
        }
        p = c + 0x30;
        for (i = 2; i < 12; i++) {
            *(int *)(p + 0x51a8) = 0x80000;
            *(int *)(p + 0x51ac) = -0x80000;
            *(int *)(p + 0x51b0) = 0x8000;
            *(u8 *)(p + 0x51ba) = 1;
            *(u8 *)(p + 0x51bc) = 0;
            p += 0x18;
        }
    } else {
        p = c;
        for (i = 0; i < 12; i++) {
            slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6 >> 15) + 1;
            for (;;) {
                q = (u8 *)(((int)c + slot + 0x5330));
                if (*q < 2) {
                    *(u8 *)(p + 0x51b8) = slot;
                    *q += 1;
                    break;
                }
                slot = (int)((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 6 >> 15) + 1;
            }
            p += 0x18;
        }
        p = c;
        for (i = 0; i < 12; i++) {
            *(int *)(p + 0x51a8) = 0x80000;
            *(int *)(p + 0x51ac) = -0x80000;
            *(int *)(p + 0x51b0) = 0x8000;
            *(u8 *)(p + 0x51ba) = 1;
            *(u8 *)(p + 0x51bc) = 0;
            p += 0x18;
        }
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- func_ov006_020f4f94, 0x020f4f94, size 0x12c */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::ResetGame()
{
    char* c = (char*)this;
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
    } while (i < 12);
    p = c;
    j = 0;
    do {
        *(int*)(p + 0x52c8) = 0;
        *(int*)(p + 0x52cc) = 0;
        *(int*)(p + 0x52d0) = 0;
        *(short*)(p + 0x52d6) = 0;
        *(short*)(p + 0x52d4) = 0;
        *(char*)(p + 0x52d8) = 0;
        *(char*)(p + 0x52d9) = 0;
        j++;
        *(char*)(p + 0x52da) = 0;
        p += 0x14;
    } while (j < 3);
    mCursor.visible = 0;
    mCursor.enabled = 0;
    mCursor.frame = 0;
    mState = 0;
    mSubstate = 0;
    mRoundTimer = 0;
    mCardTimer = 0;
    mResultTimer = 0;
    mPairsFound = 0;
    mSelectedCount = 0;
    unk_5339 = 0;
    mMisses = 0;
    i = 0;
    do {
        p = c + i;
        p += 0x5000;
        *(char*)(p + 0x32c) = 0;
        i++;
        *(char*)(p + 0x32e) = 0;
    } while (i < 2);
    j = 0;
    do {
        p = (c + j) + 0x5000;
        j++;
        *(char*)(p + 0x330) = 0;
    } while (j < 7);
    mMessageVisible = 1;
    mInputSeen = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- func_ov006_020f50c0, 0x020f50c0, size 0x38 */
/* -------------------------------------------------------------------------- */
// Call a helper on self, then if the u16 counter at self+0x5322 is nonzero, decrement
// it. The bl target is a relocation wildcard; the check folds to add+ldrh and the
// laundered RMW pool-loads the offset for the predicated tail.
void dScMgMemory_c::StateExit()
{
    UpdateCards();
    if (mCardTimer)
        mCardTimer -= 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov006_020f50f8, 0x020f50f8, size 0x48 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::StateResult(){
  (this->*data_ov006_021422bc[mSubstate])();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov006_020f5140, 0x020f5140, size 0x24 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::StateJudge() {
    UpdateCards();
    JudgePair();
    CheckFinished();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- func_ov006_020f5164, 0x020f5164, size 0x4c */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::StatePlay() {
    int j = mSubstate;
    (this->*data_ov006_02142304[j])();
    UpdateCards();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 38 -- func_ov006_020f51b0, 0x020f51b0, size 0x40 */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::StateSetup(){
    SetupDifficulty();
    ShuffleCards();
    FreeGfxSlotsById(0x1d);
    func_ov006_020c1764((char *)&mShared);
    mState = 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 39 -- func_ov006_020f51f0, 0x020f51f0, size 0x4c */
/* -------------------------------------------------------------------------- */
void dScMgMemory_c::SetupDifficulty() {
    mDifficulty = 0;
    mTargetPairs = 4;
    int x = mHudScore;
    if (x >= 0xa) {
        mDifficulty = 2;
        mTargetPairs = 6;
        return;
    }
    if (x >= 5) {
        mDifficulty = 1;
        mTargetPairs = 5;
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 40 -- _ZN13dScMgMemory_c15OnGroundPoundedEv, 0x020f523c, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScMgMemory_c15OnGroundPoundedEv
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMemory_c.h"

// recovered name: dScMgMemory_c_OnGroundPounded
/* recovered: renamed to Class_Method */
/* dScMgMemory_c::OnGroundPounded - recovered from vtable slot identity */

void dScMgMemory_c::OnGroundPounded()
{
    func_ov004_020b63a0(mMaxMisses);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 41 -- _ZN13dScMgMemory_c13OnTurnIntoEggEi, 0x020f5250, size 0x74 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScMgMemory_c13OnTurnIntoEggEi
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMemory_c.h"

// recovered name: dScMgMemory_c_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* dScMgMemory_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern void FreeGfxSlotsById(int n);

int dScMgMemory_c::OnTurnIntoEgg(int /* mode */)
{
    if (mState == 3 && mSubstate == 0) {
        if (func_ov006_020c1718((int*)&mShared) == 0) return 0;
        mSubstate = 1;
        FreeGfxSlotsById(0x1d);
    }
    return mState == 4;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 42 -- _ZN13dScMgMemory_c13OnYoshiTryEatEi, 0x020f52c4, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScMgMemory_c13OnYoshiTryEatEi
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "dScMgMemory_c.h"

// recovered name: dScMgMemory_c_OnYoshiTryEat_020f52c4
/* recovered: renamed to Class_Method, vtable slot 18 -- an override of
   dScMgBase_c::OnYoshiTryEat(int). The signature must repeat the base
   declaration exactly, or mwcc appends a slot instead of overriding. */
/* data_ov004_020beb68 is declared `void *` by dScMgBase_c.h; cast at the use site. */

void dScMgMemory_c::OnYoshiTryEat(int /* arg */)
{
    char *o;
    int v;
    ResetGame();
    mState = 0;
    o = (char *)data_ov004_020beb68;
    v = 0;
    if (o != 0) v = *(int *)(o + 0xa8);
    if (v >= 3) v = 3;
    ((void (*)(void *))func_ov004_020b66d4)(o);
    data_ov004_020bc7d4 = 1;
    mMaxMisses = (unsigned char)v;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 43 -- _ZN13dScMgMemory_c6RenderEv, 0x020f5324, size 0x64 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScMgMemory_c6RenderEv
/* dScMgMemory_c::Render -- vtable slot 9, ov006 0x020f5324.
 *
 * Attributed by the vtable: dScMgMemory_c's own table is ov006 0x0213d1b8 --
 * the address include/dScMgMemory_c.h's banner uses to prove the class is not
 * the "MgMemoryMatch" its destructor pair was once named after -- and its slot
 * 9 relocates here. The signature is include/dScMgBase_c.h's own slot 9,
 * `virtual s32 Render()`.
 *
 * The two offsets that are not members are the two sub-objects the class does
 * not own: +0x4660 belongs to dScMgSingle3DBase_c and +0x4f38 is the 0x270
 * shared table five siblings use, which this class's methods never read
 * inside. Both are passed as base addresses to their own helpers. */
s32 dScMgMemory_c::Render()
{
    char *c = (char *)this;

    func_ov006_020c0aa8(c + 0x4660);
    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b6430();
    DrawMessage();
    DrawCursor();
    DrawCards();
    func_ov006_020c1804((char *)&mShared);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 44 -- _ZN13dScMgMemory_c8BehaviorEv, 0x020f5388, size 0x5c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScMgMemory_c8BehaviorEv
/* dScMgMemory_c::Behavior -- vtable slot 6, ov006 0x020f5388.
 *
 * Attributed by the vtable: dScMgMemory_c's own table is ov006 0x0213d1b8 and
 * its slot 6 relocates here. The signature is include/dScMgBase_c.h's own slot
 * 6, `virtual s32 Behavior()`.
 *
 * THE STATE TABLE IS A POINTER-TO-MEMBER ARRAY indexed by mState, the same
 * shape dScMg3DEsp_c::Behavior has with its own table. The pre-migration file
 * reached it through a local `struct C { char pad[0x5314]; int idx; }` -- a
 * hand-cut stand-in for this class whose only purpose was to name that one
 * field -- and through a one-member `Entry` wrapper that added nothing to the
 * layout. Both are gone: the receiver is the real class, the index is the real
 * member, and the array is the member pointers themselves. Byte-identical,
 * which is what makes the 0x5314 offset evidence rather than a guess. */
s32 dScMgMemory_c::Behavior()
{
    (this->*data_ov006_021422dc[mState])();
    func_ov004_020b65e4();
    func_ov006_020c19d0(&mShared);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 45 -- _ZN13dScMgMemory_c13InitResourcesEv, 0x020f53e4, size 0x120 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScMgMemory_c13InitResourcesEv
/* dScMgMemory_c::InitResources -- vtable slot 0, ov006 0x020f53e4.
 *
 * Attributed by the vtable: dScMgMemory_c's own table is ov006 0x0213d1b8 and
 * its slot 0 relocates here. The signature is include/fBase_c.h's own slot
 * 0, `virtual s32 InitResources()`.
 *
 * unk_0a8, unk_0ac and mHudScore ARE dScMgBase_c'S, not this class's, which is
 * what include/dScMgMemory_c.h's banner says about the three fields the old
 * auto-generated header declared below 0x4f38. They read as inherited members
 * here, which is the point of the migration: the pre-migration file wrote all
 * three through `*(int *)(self + 0xa8)` and could not have said whose they
 * were.
 *
 * GX::LoadOBJPltt and GXS::LoadOBJPltt are declared as namespaced functions
 * rather than as extern "C" of their mangled names, so the compiler produces
 * _ZN2GX11LoadOBJPlttEPKvjj and _ZN3GXS11LoadOBJPlttEPKvjj the ordinary way.
 * A bare extern "C" of the mangled spelling would mangle a SECOND time in a
 * //cpp translation unit -- the defect include/SharedFilePtr.h's banner
 * records -- and reference a symbol nothing defines. */
s32 dScMgMemory_c::InitResources()
{
    char *self = (char *)this;
    void *a;
    void *b;

    data_0209d45c = 0x11;
    func_ov006_0210a534();
    a = LoadFile(data_ov006_0213d0c4[GetGameLanguage()]);
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
    if (func_ov006_020c1a88(&mShared) == 0)
        return 0;
    unk_0a8 = func_ov004_020ad8b8();
    unk_0ac = unk_0a8;
    mHudScore = func_ov004_020ad878();
    func_ov004_020b66d4();
    data_ov004_020bc7d4 = 1;
    mMaxMisses = 3;
    func_ov004_020b04d0(0x20);
    func_ov004_020b682c();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 46 -- dScMgMemory_c_classInit, 0x020f5504, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol dScMgMemory_c_classInit
/* Actor-table factory: this is the measured CodeWarrior construction boundary.
 * The profile, allocation size, RTTI and vptr transition identify the allocated
 * class; the literal calls preserve the fully folded construction sequence. */
extern "C" void *dScMgMemory_c_classInit()
{
    char *scene = (char *)_ZN7fBase_cnwEj(sizeof(dScMgMemory_c));
    if (scene) {
        _ZN11dScMgBase_cC2Ev(scene);
        *(void **)scene = _ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev(scene + 0x471c);
        *(void **)scene = _ZTV13dScMgMemory_c;
        func_ov006_020c1d80(scene + 0x4f38);
    }
    return scene;
}

/* -------------------------------------------------------------------------- */
