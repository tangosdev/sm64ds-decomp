//cpp
/* Slot1 and Slot3 minigame scenes. The shared compiler unit currently covers
 * ROM 0x0210a8c0..0x0210c9e0. Slot1 Behavior is not yet byte-matching;
 * its initialization and factory retain their existing production owners.
 *
 * With defer_codegen disabled, optimisation pragmas apply at each function
 * and emitted functions follow source order. Keep the ROM order below.
 */

#include "dScMgSlot1_c.h"
#include "dScMgSlot3_c.h"
#include "Sound.h"
#include "OAM.h"
#include "dScMgBase_c.h"
#include "decl_common.h"
#include "private/ov006_slotgrid.h"
#include "types.h"
#include "PlayerInput.h"

#pragma defer_codegen off
/* Codegen is deferred by default in mwccarm 2004/b56, which is why its
 * optimisation pragmas are otherwise file-global last-wins. With it off, the
 * positional brackets below bind to the functions they enclose -- and .text is
 * then emitted in SOURCE order, so this file is written in ASCENDING ROM order.
 */

struct G2 { static void* GetBG1ScrPtr(); };

typedef void (dScMgSlot3_c::*SlotState)();

struct SlotReels {
    u8 pad[0x46c0];
    u8 reels[3][0x15];  /* symbol strip per reel */
    u8 stops[3];        /* where each reel stopped */
    u8 pad2[6];
    u8 unk_4708;
    u8 matchSymbol;
    u8 pad3;
    u8 wilds;           /* count of symbol 5, which matches anything */
};

extern "C" {
extern int data_ov004_020bc8b4;
void SetBg1Offset(int a, int b);
int GetGameLanguage(void);
void* func_02054ea8(void);
// Retained caller ABI: narrowing to the definition's u16 adds a mask in Virtual80.
unsigned int LoadCompressedFileAt(int fileID, void* target);
extern unsigned char data_0209d45c;
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
void func_ov004_020b1ba0(void* c, int delta);
void func_ov004_020b1b78(void* c, int val);
extern unsigned int func_02012790(unsigned int arg);
extern void func_ov004_020adb1c(int self);
extern int data_ov006_0213e948[];
extern void func_ov006_0210ab08(char *c, int i);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern int Sound_PlayIfNotActive(int, int, int, int);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int b, void *attr, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void func_ov004_020b1bc8(char *a0, int a1, int a2, int a3);
void func_ov004_020b1e34(char *a0, int a1, int a2, int a3);
extern void **data_ov006_0213e5ec[];
extern SlotState data_ov006_02142bdc[];
extern int LoadFile(int handle);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern void *_ZN3G2S13GetBG2CharPtrEv(void);
extern void *_ZN3G2S12GetBG2ScrPtrEv(void);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3G2x13SetBlendAlphaEPVttttj(volatile u16 *p, u16 a, u16 b, u16 c, u16 d);
extern u8 data_0209d454;
extern int data_0208ee44;
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern int func_ov006_020c221c(char *t);
extern int _ZTV19dScMgSingle3DBase_c[];
extern int _ZTV12dScMgSlot3_c[];
extern void RenderOamMainScreen(void*, int, int, int, int);
extern void* data_ov006_0213e6a8;
extern void func_ov004_020b1b08(void *c);
extern void func_ov001_020ab3f0(void *c);
void func_ov004_020af948(void* a, int b, int c, void* m);
extern void* data_ov006_0213e528[];
int TouchArea_Update(void *c, int x);
void func_ov006_0210c2d4(void *c);
void func_ov004_020b1b40(int x);
extern void func_ov001_020ab5b0(char* r0, int r1, short r2, short r3, short s4, short s5);
extern int func_ov004_020ad8b8(void);
extern int data_ov006_0213e63c[][2];
extern void* data_ov006_0213e96c[];
extern unsigned char data_ov006_0213e4d8[];
extern void func_ov004_020b2444(int a1, int a2, int num, int a4, int a5, int sel, int idx);
extern void func_ov004_020af868(void* a0, int a1, int a2, int a3, int a4, void* a5);
extern void func_ov006_0210c234(unsigned char* o);
extern void func_ov004_020ad79c(int a, int b);
extern void func_ov004_020ae274(void *c);
extern void func_ov006_0210c180(void *o);
extern void func_ov006_0210c1a8(void *o);
extern void func_ov006_0210c218(void *o, s16 x, s16 y);
extern void func_ov006_0210c278(void *o);
extern void func_ov006_0210c2c0(void *o, int v);
extern int func_ov006_0210c500(void *self);
extern int data_ov006_0213e600[];
extern u8 data_ov006_0213e4d8[];
extern s16 data_ov006_0213e654[][2];
extern s16 data_ov006_0213e656[][2];
extern s16 data_ov006_0213e4f8[][2];
extern s16 data_ov006_0213e4fa[][2];
extern struct dFdBrightness_c data_0209f61c;
extern void func_ov004_020af770(void* a0, int a1, int a2, int a3, int a4, int a5, unsigned short a6);
extern void MultiStore16(u16 val, char *dst, int nbytes);
}

// @symbol _ZN12dScMgSlot1_cD1Ev
/* ~dScMgSlot1_c() (D1, complete-object destructor). The empty body is the
   original ownership model: the compiler destroys the typed mBetIcon member,
   emitting betIcon_c's then dThIcon_c's vtable writes, before chaining to the
   dScMgBase_c base destructor. */
dScMgSlot1_c::~dScMgSlot1_c()
{
}

// @symbol _ZN12dScMgSlot1_cD0Ev
/* The compiler emits D0 from the complete-object destructor above. */

// @symbol _ZN12dScMgSlot3_cD1Ev
dScMgSlot3_c::~dScMgSlot3_c()
{
    func_ov006_020c21e4((char *)this + 0x4f38);
}

// @symbol _ZN12dScMgSlot3_cD0Ev
/* The compiler emits D0 from the complete-object destructor above. */

// @symbol _ZN12dScMgSlot3_c25OnAimedAtWithEggReturnVecEv
/* dScMgSlot3_c::OnAimedAtWithEggReturnVec - recovered from vtable slot identity.
   The name is dScMgBase_c's, transplanted down the hierarchy; the `ReturnVec`
   half of it is refuted in include/dScMgBase_c.h's slot-30 block, and this
   body is the arity witness cited there.  The read-modify-write below clobbers
   r1 before the tail call, so the base cannot be taking a second argument. */
void dScMgSlot3_c::OnAimedAtWithEggReturnVec()
{
    void *a = (void *)this;

    *(volatile unsigned short*)0x400000A = (*(volatile unsigned short*)0x400000A & 0x43) | 0x1118;
    ((dScMgBase_c *)a)->dScMgBase_c::OnAimedAtWithEggReturnVec();
}

// @symbol _ZN12dScMgSlot3_c16OnAimedAtWithEggEv
/* dScMgSlot3_c::OnAimedAtWithEgg - recovered from vtable slot identity.

   This body is the third and strongest witness that slot 29 takes no
   explicit parameter.  It CLOBBERS r1 -- the masked read-modify-write of the
   sub display-control register below compiles to `ldrh r1,[r2]; and; orr;
   strh` -- and only then tail-branches into the base with `bx ip`.  A second
   argument passed in r1 would reach dScMgBase_c::OnAimedAtWithEgg as a
   display-control word, so the base cannot be reading one. */
int dScMgSlot3_c::OnAimedAtWithEgg()
{
    void *a = (void *)this;

    *(volatile unsigned short*)0x400000A = (*(volatile unsigned short*)0x400000A & 0x43) | 0x1000;
    return ((dScMgBase_c *)a)->dScMgBase_c::OnAimedAtWithEgg();
}

// @symbol _ZN12dScMgSlot3_c9Virtual80Ev
/* dScMgSlot3_c::Virtual80 - slot 32, the only override of it in the family.

   Not an AfterClsn: the base slot has no ROM name at all, and the one the
   recovery pass borrowed belongs to dPathLiftActor_c, two forks away.  See the
   slot-32 block in include/dScMgBase_c.h.

   The base's body verbatim -- main BG1CNT reduced to 0x1000, scroll reset, BG1
   cleared from the main BG-enable shadow, this class's own language table and
   the shared screen map -- and then one more write, leaving BG1CNT at 0x1118
   instead of 0x1000: the same layer, pointed at this minigame's own character
   and screen base blocks. */
void dScMgSlot3_c::Virtual80()
{
    int idx;

    *(volatile unsigned short*)0x400000a &= ~3;
    *(volatile unsigned short*)0x400000a = (*(volatile unsigned short*)0x400000a & 0x43) | 0x1000;
    *(volatile unsigned short*)0x400000a &= ~0x40;

    SetBg1Offset(0, 0);

    data_0209d45c &= ~2;

    idx = GetGameLanguage();
    LoadCompressedFileAt(data_ov006_0213e614[idx], func_02054ea8());
    LoadCompressedFileAt(0x67, G2::GetBG1ScrPtr());

    *(volatile unsigned short*)0x400000a = (*(volatile unsigned short*)0x400000a & 0x43) | 0x1118;
}

// @symbol func_ov006_0210ab08
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210ab08(char *c, int idx);
void func_ov006_0210ab08(char *c, int idx){
    int d = (*(int*)(c + idx*4 + 0x4fe4) >> 12) / 80;
    int i;
    char *dst = c + idx*3;
    char *src = c + idx*5;
    for (i = 0; i < 3; i++) {
        *(unsigned char*)(dst + 0x5031) = *(unsigned char*)(src + d + 0x501c);
        d = (d + 1) % *(unsigned char*)(c + 0x503a);
        dst += 1;
    }
}
}

// @symbol func_ov006_0210ab90
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210ab90(void)
{
}
}

// @symbol func_ov006_0210ab94
extern "C" {  /* .c-derived member: C linkage for the whole block */

void func_ov006_0210ab94(char* c){
    if (DecIfAbove0_Byte((unsigned char*)(c + 0x503e)) == 0) {
        *(int*)(c + 0x5000) = 7;
        func_ov004_020b0a54(0x12);
        *(int*)(c + 0x5004) = 0;
    }
    if (*(unsigned char*)(c + 0x503e) >= 0x3c) return;
    {
        int* a = (int*)(((int)c + 0x4ff4));
        int* b = (int*)(((int)c + 0x4ff8));
        int* d = (int*)(((int)c + 0x4ffc));
        *a = *a + 0x10000;
        *b = *b - 0x10000;
        *d = *d + 0x10000;
    }
}
}

// @symbol func_ov006_0210ac38
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210ac38(void)
{
}
}

// @symbol func_ov006_0210ac3c
extern "C" void func_ov006_0210ac3c(char* c)
{
    if (DecIfAbove0_Byte((unsigned char*)(c + 0x503e)) == 0) {
        if (*(int*)(c + 0x5000) == 3) {
            if (*(int*)(c + 0xb4) < 0x270f)
                *(int*)(((int)c + 0xb4)) += 1;
            if (*(int*)(c + 0xb4) > *(int*)(c + 0xb8))
                *(int*)(c + 0xb8) = *(int*)(c + 0xb4);
            ((dScMgSlot3_c*)c)->OnYoshiTryEat(4);
        } else {
            if (*(int*)(c + 0xa8) <= 0) {
                *(unsigned char*)(c + 0x503e) = 0x78;
                *(int*)(c + 0x5000) = 6;
            } else {
                ((dScMgSlot3_c*)c)->OnYoshiTryEat(5);
            }
        }
    }

    if (*(int*)(c + 0x5000) == 3) {
        if (*(unsigned char*)(c + 0x503e) != 0x14)
            return;
        if (*(int*)(c + 0x5014) <= 0)
            return;
        *(int*)(((int)c + 0x5014)) -= 1;
        func_ov004_020b1ba0(c, 1);
        Sound::PlayBank2_2D(0x149);
        *(unsigned char*)(((int)c + 0x503e)) += 5;
        return;
    }

    if (*(int*)(c + 0x5000) != 4)
        return;

    {
        unsigned char b = *(unsigned char*)(c + 0x503e);
        if (b != 0x14 && b != 0x28 && b != 0x3c)
            return;
    }

    func_ov004_020b1b78(c, 1);
    Sound::PlayBank2_2D(0x14a);
}

// @symbol func_ov006_0210adac
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210adac(char *c)
{
    unsigned char buf[3];
    int i;
    int total;
    int arg;
    char *self;

    if (DecIfAbove0_Byte((unsigned char *)(c + 0x503e)) != 0)
        return;

    self = c;
    for (i = 0; i < 3; i++) {
        int j;
        buf[i] = *(unsigned char *)(self + 0x5031 + i);
        j = 1;
        do {
            int rem = (i + (((int *)(self + 0x4fe4))[j] >> 12) / 80) % *(unsigned char *)(self + 0x503a);
            if (buf[i] != ((SlotGrid *)self)->sym[j][rem]) {
                buf[i] = 5;
                break;
            }
        } while (++j < 3);
        self = (char *)((unsigned)self | (unsigned)(i - i));
    }

    *(int *)(c + 0x5010) = -1;
    total = 0;
    {
        int k;
        for (k = 0; k < 3; k++) {
            if (buf[k] == *(unsigned char *)(c + 0x503b)) {
                if (k == 1) {
                    *(int *)(c + 0x5010) = k;
                    total += 6;
                } else if (k == 0 || k == 2) {
                    *(int *)(c + 0x5010) = k;
                    total += 3;
                }
                break;
            }
        }
    }

    arg = *(int *)(c + 0xb4);
    if (total > 0) {
        *(int *)(c + 0x5014) = total;
        *(int *)(c + 0x5000) = 3;
        func_02012790(0x26);
        *(unsigned char *)(c + 0x503e) = 0x28;
        arg += 1;
    } else {
        *(int *)(c + 0x5000) = 4;
        func_02012790(0xe);
        *(unsigned char *)(c + 0x503e) = 0x50;
    }
    func_ov004_020adb1c(arg);

    *(unsigned char *)(c + 0xc3) = 0;
}
}

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
#pragma opt_strength_reduction off
#pragma opt_loop_invariants off
// @symbol func_ov006_0210af64
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210af64(char *c)
{
  u8 idx;
  int ok;
  int off;
  int i;
  int new_var;
  int *tab;
  idx = gActivePlayerSlot;
  ok = 0;
  off = idx * 4;
  new_var = 1;
  if (gTouchHeld[idx * 4])
  {
    if (gTouchEdge[off])
    {
      ok = 1;
    }
  }
  if (ok != 0)
  {
    unsigned int v = gTouchY[idx * 4];
    if ((v >= 0x40) && (v < 0x58))
    {
      if ((*((u8 *) (c + 0x502b))) == new_var)
      {
        *((u8 *) (c + 0x502b)) = 0;
      }
    }
    else
      if ((v >= 0x58) && (v < 0x70))
    {
      if ((*((u8 *) (c + 0x502c))) == 1)
      {
        *((u8 *) (c + 0x502c)) = 0;
      }
    }
    else
      if ((v >= 0x70) && (v < 0x88))
    {
      if ((*((u8 *) (c + 0x502d))) == new_var)
      {
        *((u8 *) (c + 0x502d)) = 0;
      }
    }
  }
  i = 0;
  for (; i < 3; i++)
  {
    if ((*((u8 *) ((int) (((long long) ((int) ((c + i) + 0x502e))))))) == 0)
    {
      int *p = (int *) (c + (i * 4));
      int val;
      int q;
      int q2;
      u8 *dir;
      tab = data_ov006_0213e948;
      p = (int *) ((int) (((long long) ((int) (((int) p) + 0x4fe4)))));
      val = *p;
      ;
      q = (val >> 12) / 0x50;
      if ((*((u8 *) ((int) (((long long) ((int) ((c + i) + 0x5040))))))) == 1)
      {
        *p = val - tab[((*((u8 *) (c + 0x503d))) * 3) + i];
        if ((*p) < 0)
        {
          *p += *((int *) (c + 0x4ff0));
        }
      }
      else
      {
        *p = val + tab[((*((u8 *) (c + 0x503d))) * 3) + i];
        if ((*p) >= (*((int *) (c + 0x4ff0))))
        {
          *p -= *((int *) (c + 0x4ff0));
        }
      }
      if ((*((u8 *) ((c + i) + 0x502b))) == 0)
      {
        q2 = ((*p) >> 12) / 0x50;
        if (q2 != q)
        {
          if ((*((u8 *) ((int) (((long long) ((int) ((c + i) + 0x5040))))))) == new_var)
          {
            *p = (q * 0x50) << 12;
          }
          else
          {
            *p = (q2 * 0x50) << 12;
          }
          func_ov006_0210ab08(c, i);
          *((u8 *) ((int) (((long long) ((int) ((c + i) + 0x502e)))))) = 1;
          *((u8 *) ((int) (((long long) ((int) (c + 0x503d)))))) += new_var;
          *((u8 *) (c + 0x503f)) = 0;
          _ZN5Sound12PlayBank2_2DEj(0x1a9);
        }
      }
    }
  }

  *((int *) (c + 0x5008)) = Sound_PlayIfNotActive(*((int *) (c + 0x5008)), 2, 0x1a8, 0);
  if ((*((u8 *) (c + 0x503d))) >= 3u)
  {
    *((u8 *) (c + 0x503e)) = 0x1e;
    *((int *) (c + 0x5000)) = 2;
  }
}
}

#pragma opt_loop_invariants on
#pragma opt_strength_reduction on

// @symbol func_ov006_0210b1fc
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_020c1f04(char *c, int *src);
void func_ov004_020b0cac(int c, int a1, int a2, int a3, int arg5, short arg6);
void func_ov006_020c1ef8(int *p);
void FreeGfxSlotsById(int arg);

void func_ov006_0210b1fc(char *p)
{
    if (*(int *)(p + 0x500c) == 0x50) {
        func_ov006_020c1f04(p + 0x4f38, 0);
        func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    }

    if (*(int *)(p + 0x500c) > 0) {
        *(int *)(((int)p + 0x500c)) -= 1;
        if (*(int *)(p + 0x500c) != 0)
            return;
        *(unsigned char *)(p + 0x503b) = *(unsigned char *)(p + 0x503c);
        func_ov006_020c1ef8((int *)(p + 0x4f38));
        return;
    }

    *(unsigned char *)(((int)p + 0x503e)) -= 1;
    if (*(unsigned char *)(p + 0x503e) != 0)
        return;

    FreeGfxSlotsById(0x1d);

    {
        int *q = (int *)(p + 0x5000);
        if (*(unsigned char *)(p + 0xc4) == 0) {
            *(unsigned char *)(p + 0xc3) = 1;
            *(unsigned char *)(p + 0xc4) = 1;
            *(short *)(p + 0xc0) = 0;
        }
        *q += 1;
    }
}
}

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
#pragma opt_strength_reduction off
// @symbol _ZN12dScMgSlot3_c13OnYoshiTryEatEi
void dScMgSlot3_c::OnYoshiTryEat(int mode)
{
    char *c = (char *)this;

    if (mode == 3 || mode == 0x12) {
        *(int *)(c + 0xa8) = 0xc;
        *(int *)(c + 0xac) = *(int *)(c + 0xa8);
        *(int *)(c + 0xbc) = *(int *)(c + 0x5004);
        if (*(u32 *)(c + 0xbc) > 0x270e) {
            *(int *)(c + 0xbc) = 0x270e;
        }
        *(int *)(c + 0xb4) = 0;
        func_ov004_020adb1c(*(int *)(c + 0xb4));
        func_ov004_020b0cac(0xd, 0x80, 0xa8, 1, -1, 0xd);
    } else if (mode == 4) {
        if (*(int *)(c + 0x5004) < 5) {
            *(int *)(int)(c + 0x5004) += 1;
        }
        *(int *)(int)(c + 0xbc) += 1;
        if (*(u32 *)(c + 0xbc) > 0x270e) {
            *(int *)(c + 0xbc) = 0x270e;
        }
    }

    if (mode == 3 || mode == 0x12 || mode == 4) {
        int v;
        int v2;
        int i;
        int j;
        u8 k;
        int m;

        v = *(int *)(c + 0x5004);
        if (v <= 0) {
            *(u8 *)(c + 0x503a) = 3;
        } else if (v <= 2) {
            *(u8 *)(c + 0x503a) = 4;
        } else {
            *(u8 *)(c + 0x503a) = 5;
        }

        v2 = *(int *)(c + 0x5004);
        if (v2 <= 0 || v2 == 1 || v2 == 3) {
            *(u8 *)(c + 0x5040) = 0;
            *(u8 *)(c + 0x5041) = 1;
            *(u8 *)(c + 0x5042) = 0;
        } else if (v2 == 2 || v2 == 4) {
            *(u8 *)(c + 0x5040) = 1;
            *(u8 *)(c + 0x5041) = 0;
            *(u8 *)(c + 0x5042) = 1;
        } else {
            for (i = 0; i < 3; i++) {
                if ((u8)(((u32)RandomIntInternal(&data_0209e650) >> 16) & 1)) {
                    *(u8 *)(c + i + 0x5040) = 1;
                } else {
                    *(u8 *)(c + i + 0x5040) = 0;
                }
            }
        }

        *(u8 *)(c + 0x503c) = ((u32)RandomIntInternal(&data_0209e650) >> 16) % *(u8 *)(c + 0x503a);
        if (mode != 4) {
            *(int *)(c + 0x500c) = 0;
            *(u8 *)(c + 0x503b) = *(u8 *)(c + 0x503c);
        } else {
            *(int *)(c + 0x500c) = 0x50;
            func_02012790(0x1aa);
        }

        {
            char *w = c;
            for (j = 0; j < 3; j++, w += 5) {
                for (k = 0; k < *(u8 *)(c + 0x503a); k++) {
                    if (j == 1) {
                        *(u8 *)(c + k + 0x5021) = *(u8 *)(c + 0x503a) - k - 1;
                    } else {
                        *(u8 *)(w + k + 0x501c) = k;
                    }
                }
            }
        }
        *(int *)(c + 0x4ff0) = (*(u8 *)(c + 0x503a) * 0x50) << 12;
    }

    {
        int j2;
        int k2;
        int m2;
        char *w2 = c;
        for (j2 = 0; j2 < 3; j2++, w2 += 3) {
            *(u8 *)(c + j2 + 0x502b) = 1;
            *(u8 *)(c + j2 + 0x502e) = 0;
            for (k2 = 0; k2 < 3; k2++) {
                *(u8 *)(w2 + k2 + 0x5031) = *(u8 *)(c + 0x503a);
            }
        }
        for (m2 = 0; m2 < 3; m2++) {
            *(int *)(c + m2 * 4 + 0x4ff4) = 0;
        }
        *(u8 *)(c + 0x503d) = 0;
        *(u8 *)(c + 0x503e) = 0x3c;
        *(int *)(c + 0x5000) = 0;
        *(int *)(c + 0x5010) = -1;
        *(int *)(c + 0x5014) = 0;
    }
}

#pragma opt_strength_reduction on

// Bracketed, not file-global: with #pragma defer_codegen off these bind to
// the functions between them. Measured -- this member DIFFs without them and
// the members outside them DIFF with them applied file-wide.
#pragma opt_strength_reduction off
// @symbol _ZN12dScMgSlot3_c6RenderEv
/* dScMgSlot3_c::Render -- vtable slot 9.
 *
 * Attributed by the ROM's vtable: the second of the two slots where this class's
 * table differs from dScMgSingle3DBase_c's. The old file's `recovered name:`
 * comment agreed, and here it is right.
 *
 * Draws the three reels -- the win-line pass at state 6 and the scrolling pass
 * otherwise -- then the payout markers, the dMeter_c, and the two swinging lamps whose
 * angles are mLamp1Angle/mLamp2Angle.
 *
 * The pragma is load-bearing, not tidying; the `(int)` launder on the two indexed
 * reads at 0x4fe4/0x4ff4 is there for the same reason. Only the receiver changed:
 * `self` is `this` now instead of a char* parameter cast back to the class. */
s32 dScMgSlot3_c::Render()
{
    char *c = (char *)this;
    struct dScMgSlot3_c *self = this;
    int i, j;

    func_ov006_020c1eb4(c + 0x4660);
    func_ov006_020c201c(c + 0x4f38);

    if (self->mReelDrawY > 0) {
        for (i = 0; i < 3; i++) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov006_0213e9a4[self->mWinSymbol * 3 + i], 0x80, self->mReelDrawY + 0x10, -1, 2, 0x1000, 0x1000, 0, 1);
        }
        for (i = 0; i < 3; i++) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov006_0213e9a4[self->unk_503c * 3 + i], 0x80, self->mReelDrawY + 0x60, -1, 2, 0x1000, 0x1000, 0, 1);
        }
    } else {
        for (i = 0; i < 3; i++) {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, data_ov006_0213e9a4[self->mWinSymbol * 3 + i], 0x80, 0x60, -1, 2, 0x1000, 0x1000, 0, 1);
        }
    }

    if (self->mState != 7) {
        if (self->mState == 6) {
            int i2, j2;
            char *p = c;
            for (i2 = 0; i2 < 3; i2++) {
                int y;
                for (j2 = 0, y = 0x30; j2 < 3; j2++) {
                    Hud_RenderSprite(data_ov006_0213e9a4[*(u8 *)(p + j2 + 0x5031) * 3 + i2],
                                        y - (*(int *)((int)c + i2 * 4 + 0x4ff4) >> 12), 0x60, -1, -1);
                    y += 0x50;
                }
                p += 3;
            }
        } else {
            char *p = c;
            int row, rem;
            int i2, j2;
            for (i2 = 0; i2 < 3; i2++) {
                int a = *(int *)(((int)c + i2 * 4 + 0x4fe4)) >> 12;
                int y;
                row = a / 0x50;
                rem = a % 0x50;
                for (j2 = 0, y = 0x30; j2 < 4; j2++) {
                    Hud_RenderSprite(data_ov006_0213e9a4[*(u8 *)(p + row + 0x501c) * 3 + i2],
                                        y - rem, 0x60, -1, -1);
                    row = (row + 1) % self->mStripLength;
                    y += 0x50;
                }
                p += 5;
            }
        }
    }

    if (self->unk_503d < 3 && (self->mFrameCounter & 0x20)) {
        int slot = 3;
        int ok = 1;
        int i3, j3;
        char *p = c;
        for (i3 = 0; i3 < 3; i3++) {
            if (*(u8 *)(c + i3 + 0x502e) != 0) {
                if (slot < 3) {
                    if (self->mWinSymbol != *(u8 *)(p + slot + 0x5031))
                        ok = 0;
                } else {
                    u8 t = self->mWinSymbol;
                    for (j3 = 0; j3 < 3; j3++) {
                        if (t == *(u8 *)(p + j3 + 0x5031)) {
                            slot = j3;
                            break;
                        }
                        if (j3 == 2)
                            ok = 0;
                    }
                }
            }
            if (ok == 0)
                break;
            p += 3;
        }
        if (ok != 0 && slot < 3) {
            int count = 0;
            int i4;
            int y = 0x4c;
            for (i4 = 0; i4 < 3; i4++) {
                if (*(u8 *)(c + i4 + 0x502e) != 0) {
                    Hud_RenderSprite(&data_ov006_0213e5dc, slot * 0x50 + 0x30, y, -1, -1);
                    count++;
                }
                y += 0x18;
            }
            if ((self->mFrameCounter & 0x3f) == 0x20) {
                _ZN5Sound12PlayBank2_2DEj(count > 1 ? 0x1ac : 0x1ab);
            }
        }
    }

    func_ov004_020b1bc8(c, 0xc, 0xc, 0);
    func_ov004_020b1e34(c, 0xe0, 0x14, 1);

    if (self->mState == 3 && self->mWinColumn >= 0) {
        func_ov004_020af948(data_ov006_0213e5ec[GetGameLanguage()][2], self->mWinColumn * 0x50 + 0x20, 0x28, 0);
        func_ov004_020af948(data_ov006_0213e5ec[GetGameLanguage()][13], self->mWinColumn * 0x50 + 0x30, 0x28, 0);
        if (self->mWinColumn == 1) {
            func_ov004_020b2444(self->mWinColumn * 0x50 + 0x40, 0x28, 6, 0, 0, 0, 0x14);
        } else {
            func_ov004_020b2444(self->mWinColumn * 0x50 + 0x40, 0x28, 3, 0, 0, 0, 0x14);
        }
    } else if (self->mState == 4 && self->mWinColumn < 0) {
        func_ov004_020af948(data_ov006_0213e5ec[GetGameLanguage()][2], 0x70, 0x28, 0);
        func_ov004_020af948(data_ov006_0213e5ec[GetGameLanguage()][14], 0x80, 0x28, 0);
        func_ov004_020b2444(0x90, 0x28, 3, 0, 0, 0, 0x28);
    }

    func_ov004_020afb20(data_ov006_0213ac30, 0x18, 0x30, -1, 0, 0x1000, self->mLamp1Angle);
    func_ov004_020afb20(data_ov006_0213ac3c, 0x40, 0x10, -1, 0, 0x1000, self->mLamp2Angle);

    return 1;
}

#pragma opt_strength_reduction on

// @symbol _ZN12dScMgSlot3_c8BehaviorEv
/* dScMgSlot3_c::Behavior -- vtable slot 6.
 *
 * Attributed by the ROM's vtable: one of the two slots where this class's table
 * differs from dScMgSingle3DBase_c's. The old file's `recovered name:` comment
 * agreed, and here it is right.
 *
 * Steps the reel state machine through the pointer-to-member table at
 * data_ov006_02142bdc, ticks the frame counter at 0x503f, spins the two reel
 * offsets while state 1 is running, and flips the blend bit in data_0209d45c off a
 * coin toss so the machine's lights flicker.
 *
 * The state table uses this class's pointer-to-member type.
 */
s32 dScMgSlot3_c::Behavior()
{
    dScMgSlot3_c *self = this;
    int i;
    unsigned char t;

    unsigned char *pc;
    unsigned short *ph1;
    unsigned short *ph2;

    (self->*data_ov006_02142bdc[self->mState])();

    pc = (unsigned char *)((char *)self + 0x503f);
    *pc = *pc + 1;
    func_ov006_020c2144((char *)self + 0x4f38);

    for (i = 0; i < 3; i++) {
        if (self->mState == 1) {
            ph1 = (unsigned short *)((char *)self + 0x5018);
            ph2 = (unsigned short *)((char *)self + 0x501a);
            *ph1 = *ph1 - 0x200;
            *ph2 = *ph2 - 0x400;
            break;
        }
    }

    t = ((unsigned int)RandomIntInternal(&data_0209e650) >> 16) & 1;
    if (t)
        data_0209d45c |= 2;
    else
        data_0209d45c &= ~2;

    return 1;
}

// @symbol _ZN12dScMgSlot3_c13InitResourcesEv
/* Load the reel assets and initialise the scene state. */
s32 dScMgSlot3_c::InitResources()
{
    char *c = (char *)this;

    *(volatile u16 *)0x400000a = (*(volatile u16 *)0x400000a & 0x43) | 0x1118;
    *(volatile u16 *)0x400000a = *(volatile u16 *)0x400000a & ~0x40;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0x1218;
    *(volatile u16 *)0x400000c = *(volatile u16 *)0x400000c & ~0x40;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1418;
    *(volatile u16 *)0x400000e = *(volatile u16 *)0x400000e & ~0x40;

    *(volatile u16 *)0x4000008 = *(volatile u16 *)0x4000008 & ~3;
    *(volatile u16 *)0x400000a = *(volatile u16 *)0x400000a & ~3;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & ~3) | 1;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & ~3) | 3;

    LoadCompressedFileAt(0x77, (void *)func_02054d88());

    {
        int h = LoadFile(0x78);
        _ZN2GX10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
        _ZN3GXS10LoadBGPlttEPKvjj((const void *)h, 0x60, 0x1a0);
        Deallocate((void *)h);
    }

    LoadCompressedFileAt(0x75, _ZN2G212GetBG1ScrPtrEv());
    LoadCompressedFileAt(0x7a, _ZN2G212GetBG2ScrPtrEv());

    {
        int h = LoadFile(0x79);
        func_02056314((void *)h, 0, 0x800);
        Deallocate((void *)h);
    }

    LoadCompressedFileAt(0xeb, (void *)0x6400000);

    data_0209d45c = 0x1f;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & ~3) | 3;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0x1218;
    *(volatile u16 *)0x400100c = *(volatile u16 *)0x400100c & ~0x40;

    LoadCompressedFileAt(0x77, _ZN3G2S13GetBG2CharPtrEv());
    LoadCompressedFileAt(0x76, _ZN3G2S12GetBG2ScrPtrEv());
    LoadCompressedFileAt(0xeb, (void *)0x6600000);

    {
        int h = LoadFile(0xec);
        _ZN4CP1527FlushAndInvalidateDataCacheEjj((u32)h, 0xe0);
        _ZN2GX11LoadOBJPlttEPKvjj((const void *)h, 0, 0xe0);
        _ZN3GXS11LoadOBJPlttEPKvjj((const void *)h, 0, 0xe0);
        Deallocate((void *)h);
    }

    data_0209d454 = 0x14;
    _ZN3G2x13SetBlendAlphaEPVttttj((volatile u16 *)0x4000050, 0, 0xc, 0xc, 4);

    func_ov006_020c2154(c + 0x4f38);
    func_ov006_020c1eb4(c + 0x4660);

    *(int *)(c + 0x5004) = *(int *)(c + 0xbc);

    ((dScMgSlot3_c *)c)->OnYoshiTryEat(3);

    *(u8 *)(c + 0x5000 + 0x3b) = *(u8 *)(c + 0x5000 + 0x3c);
    *(int *)(c + 0x5000 + 0xc) = 0;

    for (int i = 0; i < 3; i++) {
        int rnd = RandomIntInternal(&data_0209e650);
        u32 divisor = *(u8 *)(c + 0x5000 + 0x3a);
        u32 val = ((u32)rnd >> 16) % divisor;
        ((u32 *)(c + 0x4fe4))[i] = val * 0x50000;
    }

    data_ov004_020bc88c = 0x80;
    data_ov004_020bc860 = 0xa0;
    data_ov004_020bc878 = 0x80;
    data_ov004_020bc890 = 0xa0;
    data_ov004_020bc8b8 = 0x80;
    data_ov004_020bc8b4 = 0x60;
    data_ov004_020bc888 = 0x80;
    data_ov004_020bc864 = ~0x1b;

    func_ov004_020b04d0(0x20);

    data_0208ee44 = 1;

    *(u16 *)(c + 0x5018) = 0;
    *(u16 *)(c + 0x501a) = 0;
    return 1;
}

// @symbol dScMgSlot3_c_classInit
/* STILL MACHINE-SHAPED (audit 2026-09-18) -- byte-exact; what blocks each part:
 *  1 func_ov006_*                unnamed in config symbols.txt; each needs a
 *                                coined, behaviour-justified name.
 *  3 ctor/dtor/op-new call(s)    C1/C2/D0/D1/D2 is not expressible
 *                                in C++ source; only a real ctor emits it.
 *  2 _ZTV vptr store(s)          stands in for the ctor that would emit it.
 */
/* Reconstructed source-style name: SM64DS proves dScMgSlot3_c through RTTI,
 * allocation size, vtable identity, and the MG_SLOT3 registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov006_0210c120. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
int *dScMgSlot3_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(0x5044);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)_ZTV19dScMgSingle3DBase_c;
        _ZN8Particle10SysTrackerC1Ev((char *)p + 0x471c);

        p[0] = (int)&_ZTV12dScMgSlot3_c[2];
        func_ov006_020c221c((char *)p + 0x4f38);
    }
    return p;
}
}

// @symbol func_ov006_0210c180
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c180(void *raw){
    char *blink = (char *)raw;
    *(int *)blink = 0x48;
    *(u8 *)(blink + 4) = 1;
    data_0209d454 |= 2;
}
}

// @symbol func_ov006_0210c1a8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c1a8(void *o_)
{
    int *timer = (int *)o_;
    if (*timer <= 0)
        return;
    *timer -= 1;
    if ((*timer & 7) != 0)
        return;
    *((u8 *)timer + 4) ^= 1;
    if (*((u8 *)timer + 4) != 0)
        data_0209d454 |= 2;
    else
        data_0209d454 &= ~2;
}
}

// @symbol func_ov006_0210c208
/* 0x469c blinks bit 1 of data_0209d454 the same way: a timer and an on
 * flag. This clears it, 0210c180 starts it and 0210c1a8 ticks it. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c208(char *blink)
{
    *(int *)(blink + 0x0) = 0;
    *(char *)(blink + 0x4) = 0;
}
}

// @symbol func_ov006_0210c218
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c218(void *raw, s16 x, s16 y)
{
    u8 *sprite = (u8 *)raw;
    *(u16 *)(sprite + 0) = x;
    *(u16 *)(sprite + 2) = y;
    *(int *)(sprite + 4) = 0x48;
    *(u8 *)(sprite + 8) = 1;
}
}

// @symbol func_ov006_0210c234
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c234(unsigned char* raw){
    if (*(u8 *)(raw + 8) == 0) return;
    RenderOamMainScreen(&data_ov006_0213e6a8, *(short *)raw, *(short *)(raw + 2), -1, -1);
}
}

// @symbol func_ov006_0210c278
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c278(void *o_)
{
    u8 *raw = (u8 *)o_;
    if (*(int *)(raw + 4) <= 0) return;
    int *timer = (int *)(raw + 4);
    *timer -= 1;
    if ((*(int *)(raw + 4) & 7) == 0) {
        *(raw + 8) ^= 1;
    }
}
}

// @symbol func_ov006_0210c2b0
/* 0x4684 and 0x4690 each hold a blinking sprite: s16 x and y, a timer at
 * +4 and a visible flag at +8. This clears one, 0210c218 starts one, 0210c278
 * ticks it and 0210c234 draws it. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c2b0(char *sprite)
{
    *(int *)(sprite + 0x4) = 0;
    *(char *)(sprite + 0x8) = 0;
}
}

// @symbol func_ov006_0210c2c0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c2c0(void *raw, int value) {
    int *words = (int *)raw;
    words[8] = (words[7] * value) << 2;
}
}

// @symbol func_ov006_0210c2d4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c2d4(void *c_)
{
    dThIcon_c *icon = (dThIcon_c *)c_;
    void *scene;
    int count;
    unsigned int sid;

    if (icon->unk_01c >= 3) {
        return;
    }
    if (icon->unk_010 != 0) {
        return;
    }
    scene = data_ov004_020beb68;
    count = (scene != 0) ? *(int *)((char *)scene + 0xa8) : 0;
    if (count <= 0) {
        return;
    }
    icon->unk_01c += 1;
    func_ov004_020b1b08((void *)1);
    func_ov001_020ab3f0(icon);
    sid = 0x163;
    Sound::PlayBank2_2D(sid);
}
}

// @symbol func_ov006_0210c354
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c354(void *raw)
{
    dThIcon_c *icon = (dThIcon_c *)raw;
    icon->unk_01c = 0;
    icon->unk_020 = 0;
    if (icon->unk_014 >= 1)
        icon->unk_011 = 1;
}
}

// @symbol _ZN12dScMgSlot1_c9betIcon_c6RenderEv
void dScMgSlot1_c::betIcon_c::Render()
{
    OAM::RenderSub((OamAttr *)data_ov006_0213e528[unk_010], unk_004, unk_006, -1, 1);
    int i;
    int y = 0xb0;
    int zero = 0;
    for (i = 0; i < unk_01c; i++) {
        func_ov004_020af948(data_ov006_0213e5ec[GetGameLanguage()][2], 0xb0 + i * 0x10, y, (void *)zero);
    }
}

// @symbol _ZN12dScMgSlot1_c9betIcon_c8BehaviorEv
void dScMgSlot1_c::betIcon_c::Behavior()
{
    int val;
    if (TouchArea_Update(this, -1))
        func_ov006_0210c2d4(this);
    val = unk_020;
    if (val != 0) {
        if ((val & 3) == 0) {
            func_ov004_020b1b40(1);
            Sound::PlayBank2_2D(0x149);
        }
        unk_020 -= 1;
    }
    dThIcon_c::Behavior();
}

// @symbol func_ov006_0210c478
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c478(char *icon) {
    func_ov001_020ab5b0(icon, 1, 0x90, 0xb0, 0x10, 8);
    *(int *)(icon + 0x18) = 6;
}
}

// @symbol _ZN12dScMgSlot1_c19OnHitFromUnderneathEv

int dScMgSlot1_c::OnHitFromUnderneath()
{
    dScMgBase_c::OnHitFromUnderneath();
    SetSubBg1Offset(0x100, 0);
}

// @symbol _ZN12dScMgSlot1_c15OnHitByMegaCharEv
/* Slot 27. Takes no argument: the ROM body never reads r1. The qualified
 * base call is the ROM's direct branch. */
void dScMgSlot1_c::OnHitByMegaChar()
{
    SetSubBg1Offset(0, 0);
    dScMgBase_c::OnHitByMegaChar();
}

// @symbol func_ov006_0210c500
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_0210c500(void *self)
{
    struct SlotReels *p = (struct SlotReels *)self;
    int ok;
    int lead;
    int i;

    p->wilds = 0;
    ok = 1;
    for (i = 0; i < 3; i++) {
        if (p->reels[i][p->stops[i]] == 5) {
            p->wilds++;
        } else {
            lead = p->reels[i][p->stops[i]];
            break;
        }
    }
    if (p->wilds >= 3) {
        lead = 5;
    } else if (i < 2) {
        for (i = i + 1; i < 3; i++) {
            u8 sym = p->reels[i][p->stops[i]];
            if (sym == 5) {
                p->wilds++;
            } else if (lead != sym) {
                ok = 0;
            }
        }
    }
    if (ok) {
        if (p->unk_4708 < 2) {
            u8 *q = (u8 *)p + 0x4708;
            *q += 1;
        }
        p->matchSymbol = lead;
    }
    return ok;
}
}

// @symbol func_ov006_0210c638
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c638(void *self)
{
    char *raw = (char *)self;
    *(s8 *)(raw + 0x4706) = -1;
    *(u8 *)(raw + 0x4707) = 1;
    *(u8 *)(raw + 0x4708) = 0;
    dScMgSlot1_c *scene = (dScMgSlot1_c *)self;
    scene->unk_0a8 = func_ov004_020ad8b8();
    scene->unk_0ac = scene->unk_0a8;
}
}

// @symbol _ZN12dScMgSlot1_c13OnYoshiTryEatEi
/* Slot 18. 0x4706 is padding in the header, so it is written by offset. */
void dScMgSlot1_c::OnYoshiTryEat(int i)
{
    char *raw = (char *)this;

    if (i == 4) {
        *(u8 *)(raw + 0x4706) = unk_4709;
    } else if (i == 3) {
        func_ov006_0210c638(raw);
    }
    func_ov006_0210c354(&mBetIcon);
    unk_46b4 = 0;
}

#pragma opt_strength_reduction off
// @symbol _ZN12dScMgSlot1_c6RenderEv
/* dScMgSlot1_c::Render -- the recovered symbol at this address
   (_ZN3OAM7SECONDSE, i.e. "OAM::SECONDS") was a name-recovery-heuristic
   artifact, not a real name; this function genuinely is Render (see
   include/dScMgSlot1_c.h's file banner). mBetIcon.Render() is the ROM's
   virtual call through the typed nested betIcon_c member at 0x4660.
   `this+0xa8` is inherited from further up the hierarchy than dScMgBase_c,
   so it stays a raw offset on an unsigned char* cast. */
s32 dScMgSlot1_c::Render()
{
    unsigned char* t = (unsigned char*)this;
    int i, pos, x, j;
    unsigned char* cur = t;
    unsigned char b;
    for (i = 0; i < 3; i++) {
        pos = *(t + i + 0x46ff);
        x = data_ov006_0213e63c[i][1] - (*(int*)(t + (i << 2) + 0x46a4) >> 12);
        for (j = 0; j < 2; j++) {
            Hud_RenderSprite(data_ov006_0213e96c[*(cur + pos + 0x46c0)],
                                data_ov006_0213e63c[i][0], x, -1, 3);
            pos--;
            x += 0x40;
            if (pos < 0)
                pos += 0x15;
        }
        cur += 0x15;
    }
    mBetIcon.Render();
    func_ov004_020b2444(0x70, 0xb0, *(int*)(t + 0xa8), 0, 1, 1, 0x14);
    if (*(int*)(t + 0x46b4) >= 5 && (*(t + 0x470a) != 0 || (*(t + 0x470b) != 0 && *(t + 0x470b) < 3))) {
        b = *(t + 0x470b);
        if (b != 0 && b < 3) {
            i = 0;
            if ((int)b > 0) {
                pos = 0x18;
                do {
                    func_ov004_020af770(data_ov006_0213e96c[0xb], pos, 0x30, -1, 2, 0x1000, 0);
                    pos += 0x10;
                    i++;
                } while (i < *(t + 0x470b));
            }
            func_ov004_020af868(data_ov006_0213e5ec[GetGameLanguage()][1], 0x50, 0x30, -1, 2, 0);
            func_ov004_020b2444(0x60, 0x30, *(t + 0x470b) * 2, 0, 2, 2, 0x14);
        }
        if (*(t + 0x470a) != 0) {
            i = 0;
            pos = 0x18;
            for (; i < 3; i++) {
                func_ov004_020af770(data_ov006_0213e96c[*(t + 0x4709) + 6], pos, 0x40, -1, 2, 0x1000, 0);
                pos += 0x10;
            }
            func_ov004_020af868(data_ov006_0213e5ec[GetGameLanguage()][1], 0x50, 0x40, -1, 2, 0);
            func_ov004_020b2444(0x60, 0x40, data_ov006_0213e4d8[*(t + 0x4709)], 0, 2, 2, 0x14);
        }
    }
    func_ov006_0210c234(t + 0x4684);
    func_ov006_0210c234(t + 0x4690);
    return 1;
}

#pragma opt_strength_reduction on
