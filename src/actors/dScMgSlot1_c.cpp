//cpp
/* dScMgSlot1_c -- ov006, intact translation unit.
 *
 * Licensed .text range 0x0210c180..0x0210c6c0, 18 functions, compiled by
 * mwccarm 2004/b56 and byte-verified against the ROM.
 *
 * This is a SUB-RANGE of a larger original TU. The ROM's own unit spans
 * 0x0210a8c0..0x0210d6b8 and holds dScMgSlot1_c and dScMgSlot3_c together
 * (41 functions); see this TU's manifest boundary_evidence for how that
 * boundary was derived and for the six functions that keep the full range
 * from being claimed. The same truncate-at-the-first-unmatched-function
 * shape is what src/actors/dScMgCurling2_c.cpp and
 * src/minigames/d_s_mg_hanachan.cpp already do in this overlay.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm 2004/b56 emits one
 * .text section per function, in the reverse of source order, so the
 * highest-address ROM function is written first.
 *
 * Folded from the 18 one-function legacy sources that used to carry these
 * symbols; those files are deleted by this change, so they are named here
 * by symbol rather than by a path that no longer resolves (ROM order):
 *   0x0210c180  func_ov006_0210c180
 *   0x0210c1a8  func_ov006_0210c1a8
 *   0x0210c208  func_ov006_0210c208
 *   0x0210c218  func_ov006_0210c218
 *   0x0210c234  func_ov006_0210c234
 *   0x0210c278  func_ov006_0210c278
 *   0x0210c2b0  func_ov006_0210c2b0
 *   0x0210c2c0  func_ov006_0210c2c0
 *   0x0210c2d4  func_ov006_0210c2d4
 *   0x0210c354  func_ov006_0210c354
 *   0x0210c374  _ZN12dScMgSlot1_c9betIcon_c6RenderEv
 *   0x0210c410  _ZN12dScMgSlot1_c9betIcon_c8BehaviorEv
 *   0x0210c478  func_ov006_0210c478
 *   0x0210c4b8  _ZN12dScMgSlot1_c19OnHitFromUnderneathEv
 *   0x0210c4dc  _ZN12dScMgSlot1_c15OnHitByMegaCharEv
 *   0x0210c500  func_ov006_0210c500
 *   0x0210c638  func_ov006_0210c638
 *   0x0210c674  _ZN12dScMgSlot1_c13OnYoshiTryEatEi
 */

#include "dScMgSlot1_c.h"
#include "dScMgSlot3_c.h"
#include "dScMgBase_c.h"
#include "decl_common.h"
#include "private/ov006_slotgrid.h"
#include "types.h"


/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* shadow struct 'G2' */
struct G2 { static void* GetBG1ScrPtr(); };

/* shadow namespace 'Sound' */
namespace Sound {
    void PlayBank2_2D(unsigned int id);
}

/* shadow struct 'Obj' */
struct Obj {
    virtual void v00();
    virtual void v01();
    virtual void v02();
    virtual void v03();
    virtual void v04();
    virtual void v05();
    virtual void v06();
    virtual void v07();
    virtual void v08();
    virtual void v09();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18(int a);
    virtual void v19();
    /* Reconciled from two legacy spellings of this same shadow: the
       virtual-only body (func_ov006_0210ac3c, dScMgSlot1_c::InitResources)
       and the data-member body (dScMgSlot3_c::Behavior). Offsets are the
       ROM's absolute ones, so the leading pad is one word short to leave
       room for the vptr the virtuals put at offset 0. Only `idx` is reached
       by name; every other field in the legacy data spelling was already a
       raw offset on a char* cast, and stays one. */
    char pad0[0x5000 - 4];
    int idx;
};

/* shadow typedef 'T4fe4' */
typedef struct T4fe4 {
    char pad[0x4fe4];
    int vals[3];
} T4fe4;

/* shadow typedef 'void' */
typedef void (Obj::*ObjFn)();

/* shadow struct 'T' */
struct T {
    u8 pad[0x46c0];
    u8 grid[3][0x15];
    u8 idx[3];
    u8 pad2[6];
    u8 f708;
    u8 f709;
    u8 pad3;
    u8 count;
};

extern "C" {
void SetBg1Offset(int a, int b);
int GetGameLanguage(void);
void* func_02054ea8(void);
extern unsigned char data_0209d45c;
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern "C" void func_ov004_020b1ba0(void* c, int delta);
extern "C" void func_ov004_020b1b78(void* c, int val);
extern void func_ov004_020adb1c(int self);
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0deb[];
extern int data_ov006_0213e948[];
extern void func_ov006_0210ab08(char *c, int i);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern int Sound_PlayIfNotActive(int, int, int, int);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int b, void *attr, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9);
void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
void func_ov004_020b1bc8(char *a0, int a1, int a2, int a3);
extern ObjFn data_ov006_02142bdc[];
extern int LoadFile(int handle);
extern void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern void *_ZN2G212GetBG2ScrPtrEv(void);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(u32 a, u32 b);
extern void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
extern u8 data_0209d454;
extern int data_0208ee44;
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN11dScMgBase_cC2Ev(void *);
extern void _ZN8Particle10SysTrackerC1Ev(void *);
extern int func_ov006_020c221c(char *t);
extern int _ZTV12dScMgSlot3_c[];
extern void RenderOamMainScreen(void*, int, int, int, int);
extern void* data_ov006_0213e6a8;
extern void func_ov004_020b1b08(void *c);
extern void func_ov001_020ab3f0(void *c);
extern void *data_ov004_020beb68;
void _ZN3OAM9RenderSubEP7OamAttriiii(void* a, int b, int c, int d, int e);
void func_ov004_020af948(void* a, int b, int c, void* m);
extern void* data_ov006_0213e528[];
extern void** data_ov006_0213e5ec[];
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
extern int func_0202ec9c(void *fader, int a);
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
extern struct FaderBrightness data_0209f61c;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 37 -- _ZN12dScMgSlot1_c13OnYoshiTryEatEi, 0x0210c674, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot1_c13OnYoshiTryEatEi
// recovered name: dScMgSlot1_c_OnYoshiTryEat_0210c674
/* dScMgSlot1_c::OnYoshiTryEat - recovered from vtable slot identity. Slot 18
   stays an unmigrated raw extern "C" helper (see include/dScMgSlot1_c.h),
   so this reaches its own fields via raw offsets on a char* rather than
   including the now-C++ class header -- 0x4706 is touched only here, not
   by any migrated method, same precedent dScMgPachinko2_c.h documents for
   its own offset 0xbc. */
void dScMgSlot1_c::OnYoshiTryEat(int i)
{
    char *c = (char *)this;

  if(i == 4){
    *(unsigned char*)(c + 0x4706) = *(unsigned char*)(c + 0x4709);
  } else if(i == 3){
    func_ov006_0210c638(c);
  }
  func_ov006_0210c354(c+0x4660);
  *(int*)(c + 0x46b4) = 0;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 36 -- func_ov006_0210c638, 0x0210c638, size 0x3c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c638
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c638(void *thiz_)
{
    char *thiz = (char *)thiz_;
    *(signed char *)(thiz + 0x4706) = -1;
    *(unsigned char *)(thiz + 0x4707) = 1;
    *(unsigned char *)(thiz + 0x4708) = 0;
    *(int *)(thiz + 0xa8) = func_ov004_020ad8b8();
    *(int *)(thiz + 0xac) = *(int *)(thiz + 0xa8);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 35 -- func_ov006_0210c500, 0x0210c500, size 0x138 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c500
extern "C" {  /* .c-derived member: C linkage for the whole block */
int func_ov006_0210c500(void *self)
{
    struct T *p = (struct T *)self;
    int ok;
    int first;
    int i;

    p->count = 0;
    ok = 1;
    for (i = 0; i < 3; i++) {
        if (p->grid[i][p->idx[i]] == 5) {
            p->count++;
        } else {
            first = p->grid[i][p->idx[i]];
            break;
        }
    }
    if (p->count >= 3) {
        first = 5;
    } else if (i < 2) {
        for (i = i + 1; i < 3; i++) {
            u8 c = p->grid[i][p->idx[i]];
            if (c == 5) {
                p->count++;
            } else if (first != c) {
                ok = 0;
            }
        }
    }
    if (ok) {
        if (p->f708 < 2) {
            u8 *q = (u8 *)(int)((u8 *)p + 0x4708);
            *q = *q + 1;
        }
        p->f709 = first;
    }
    return ok;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 34 -- _ZN12dScMgSlot1_c15OnHitByMegaCharEv, 0x0210c4dc, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot1_c15OnHitByMegaCharEv
/* dScMgSlot1_c::OnHitByMegaChar -- slot 27, and a REAL OVERRIDE as of this
   commit: dScMgBase_c declares the slot now, so this stops being a new virtual
   that merely happened to land on index 27 and becomes the override the
   cartridge has.  include/dScMgSlot1_c.h's banner carries the arithmetic.

   THE PARAMETER IS GONE, and that is a measurement rather than a tidy-up.  This
   body opens `mov r4, r0` and then writes r0 and r1 with zero for
   SetSubBg1Offset; no second argument register is live on entry, and
   dScMgBase_c's own body reads none either.  The `Player &player` this file
   used to carry was copied from include/dActor_c.h's slot 27, a parallel
   hierarchy -- and dActor_c.h has now been wrong on every parameter list this
   campaign has measured.  Keeping it would have been worse than cosmetic:
   `(Player &)` and `()` are different functions, so this would have become a
   NEW slot at 28, pushed OnHitFromUnderneath to 29 and put _ZTV12dScMgSlot1_c
   back into DIFFERS -- with rombuild green the whole time.

   The return type is unchanged and still `void`.  dScMgBase_c's body measures
   it directly now (an early `popne {r4,lr}; bxne lr` with no r0 set), where
   before this file could only cite Stump::OnHitByMegaChar from the other
   branch.

   The forwarding call is written qualified, `dScMgBase_c::OnHitByMegaChar()`,
   which suppresses the virtual dispatch and emits the same direct `bl` the ROM
   has.  It was previously spelled as an `extern "C"` call to the base body's
   pre-rename name, which is the only spelling that was available while the
   base had not declared the member. */
void dScMgSlot1_c::OnHitByMegaChar()
{
    SetSubBg1Offset(0, 0);
    dScMgBase_c::OnHitByMegaChar();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 33 -- _ZN12dScMgSlot1_c19OnHitFromUnderneathEv, 0x0210c4b8, size 0x24 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot1_c19OnHitFromUnderneathEv
/* dScMgSlot1_c::OnHitFromUnderneath -- slot 28, and a REAL OVERRIDE as of this
   commit: dScMgBase_c declares the slot now, so this stops being a new virtual
   that merely happened to land on index 28 and becomes the override the
   cartridge has.  It was the last virtual in this class that mwcc numbered for
   itself; include/dScMgSlot1_c.h's banner carries the arithmetic that retires
   with it.

   THE PARAMETER IS GONE, and unlike slot 27's it comes off on the base's
   evidence alone.  dScMgBase_c's body at ov004:0x020af04c writes r1
   (`add r1, r4, #0x4000`) before it ever reads it and touches no other
   argument register, so nothing in the family consumes a second argument.
   This body cannot corroborate that the way the slot-27 override did: it calls
   the base as its very first act, so a second argument would ride through r1
   untouched and leave no trace.  The `dActor_c &other` this file used to carry
   was copied from include/dActor_c.h's slot 28, a parallel hierarchy, and
   dActor_c.h has been wrong on every parameter list this campaign has
   measured.  What is NOT invisible is DISAGREEMENT: `(dActor_c &)` and `()`
   are different functions, so keeping it would have made this a new slot at 29
   and put _ZTV12dScMgSlot1_c back into DIFFERS -- with rombuild green the
   whole time.

   THE RETURN TYPE IS THE ONE THING HERE THAT IS NOT MEASURED, and slot 28 is
   the first in this campaign where that is true.  Neither body sets r0
   deliberately: the base falls out of a virtual call it has just compared
   against zero, and this one returns whatever SetSubBg1Offset left behind.  No
   caller loads vtable+0x70 anywhere in ov004 or ov006, so nothing consumes a
   result either.  `int` is include/dActor_c.h's, kept because it is what this
   file already carried and because dActor_c.h's RETURN types have held up
   where its parameter lists have not -- a hint, not a measurement.  `void`
   compiles to the same bytes.

   The forwarding call is written qualified, dScMgBase_c::OnHitFromUnderneath(),
   which suppresses the virtual dispatch and emits the same direct `bl` the ROM
   has -- straight to ov004:0x020af04c, not through ov006's veneer.  It was
   previously spelled as an extern "C" call to the base body's pre-rename name,
   which is the only spelling that was available while the base had not
   declared the member. */
int dScMgSlot1_c::OnHitFromUnderneath()
{
    dScMgBase_c::OnHitFromUnderneath();
    SetSubBg1Offset(0x100, 0);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 32 -- func_ov006_0210c478, 0x0210c478, size 0x40 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c478
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c478(char *c) {
    func_ov001_020ab5b0(c, 1, 0x90, 0xb0, 0x10, 8);
    *(int*)(c + 0x18) = 6;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 31 -- _ZN12dScMgSlot1_c9betIcon_c8BehaviorEv, 0x0210c410, size 0x68 */
/* -------------------------------------------------------------------------- */
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
            _ZN5Sound12PlayBank2_2DEj(0x149);
        }
        unk_020 -= 1;
    }
    dThIcon_c::Behavior();
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 30 -- _ZN12dScMgSlot1_c9betIcon_c6RenderEv, 0x0210c374, size 0x9c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12dScMgSlot1_c9betIcon_c6RenderEv
void dScMgSlot1_c::betIcon_c::Render()
{
    _ZN3OAM9RenderSubEP7OamAttriiii(
        data_ov006_0213e528[unk_010], unk_004, unk_006, -1, 1);
    int i;
    int sb = 0xb0;
    int z = 0;
    for (i = 0; i < unk_01c; i++) {
        func_ov004_020af948(data_ov006_0213e5ec[GetGameLanguage()][2], 0xb0 + i*0x10, sb, (void*)z);
    }
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 29 -- func_ov006_0210c354, 0x0210c354, size 0x20 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c354
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c354(void *p_)
{
    char *p = (char *)p_;
    *(int *)(p + 0x1c) = 0;
    *(int *)(p + 0x20) = 0;
    if (*(int *)(p + 0x14) >= 1)
        *(unsigned char *)(p + 0x11) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 28 -- func_ov006_0210c2d4, 0x0210c2d4, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c2d4
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c2d4(void *c_)
{
    char *c = (char *)c_;
    void *p;
    int v;
    unsigned int sid;

    if (*(int *)(c + 0x1c) >= 3) {
        return;
    }
    if (*(unsigned char *)(c + 0x10) != 0) {
        return;
    }
    p = data_ov004_020beb68;
    v = (p != 0) ? *(int *)((char *)p + 0xa8) : 0;
    if (v <= 0) {
        return;
    }
    *(int *)(((int)c + 0x1c)) =
        *(int *)(((int)c + 0x1c)) + 1;
    func_ov004_020b1b08((void *)1);
    func_ov001_020ab3f0(c);
    sid = 0x163;
    _ZN5Sound12PlayBank2_2DEj(sid);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 27 -- func_ov006_0210c2c0, 0x0210c2c0, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c2c0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c2c0(void *o, int v) {
  int *p = (int *)o;
  p[8] = (p[7] * v) << 2;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 26 -- func_ov006_0210c2b0, 0x0210c2b0, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c2b0
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c2b0(char *p)
{
    *(int *)(p + 0x4) = 0;
    *(char *)(p + 0x8) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 25 -- func_ov006_0210c278, 0x0210c278, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c278
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c278(void *o_)
{
    unsigned char *o = (unsigned char *)o_;
    if (*(int*)(o + 4) <= 0) return;
    int *p = (int*)(((int)o + 4));
    *p = *p - 1;
    if ((*(int*)(o + 4) & 7) == 0) {
        *((unsigned char*)(((int)o + 8))) ^= 1;
    }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 24 -- func_ov006_0210c234, 0x0210c234, size 0x44 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c234
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c234(unsigned char* o){
  if(*(unsigned char*)(o+8)==0) return;
  RenderOamMainScreen(&data_ov006_0213e6a8, *(short*)o, *(short*)(o+2), -1, -1);
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 23 -- func_ov006_0210c218, 0x0210c218, size 0x1c */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c218
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c218(void *p_, s16 a, s16 b)
{
    unsigned char *p = (unsigned char *)p_;
    *(unsigned short *)(p + 0) = a;
    *(unsigned short *)(p + 2) = b;
    *(int *)(p + 4) = 0x48;
    *(unsigned char *)(p + 8) = 1;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 22 -- func_ov006_0210c208, 0x0210c208, size 0x10 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c208
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c208(char *p)
{
    *(int *)(p + 0x0) = 0;
    *(char *)(p + 0x4) = 0;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 21 -- func_ov006_0210c1a8, 0x0210c1a8, size 0x60 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c1a8
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c1a8(void *o_)
{
    int *o = (int *)o_;
    if (*o <= 0)
        return;
    *o = *o - 1;
    if ((*o & 7) != 0)
        return;
    *((unsigned char *)(((int)o + 4))) ^= 1;
    if (*((unsigned char *)o + 4) != 0)
        data_0209d454 |= 2;
    else
        data_0209d454 &= ~2;
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 20 -- func_ov006_0210c180, 0x0210c180, size 0x28 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov006_0210c180
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov006_0210c180(void *c_){
  char *c = (char *)c_;
  *(int*)c = 0x48;
  *(unsigned char*)(c+4) = 1;
  data_0209d454 |= 2;
}
}

