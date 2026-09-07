//cpp
/* dScGameOver_c -- the game-over screen, one of dScene_c's ten direct
 * children.  Mario is out of lives; this scene drops the eight "GAME OVER"
 * glyphs in from off screen and then offers the yes/no continue prompt.
 *
 * ov003, .text 0x020b0580 .. 0x020b1118, 9 functions, reconstructed from 9
 * one-function legacy sources by tools/tubuild.py create and reconciled by
 * hand.  See include/dScGameOver_c.h for the class's own evidence trail --
 * where its fields end, why the destructor is inline, and why InitResources
 * rather than the destructor is this TU's key function.
 *
 * Seven of the 9 are this class's own members: vtable slots 0, 3, 6, 9, 12,
 * 16 and 17, exactly the seven dScene_c gives every direct child (the class
 * adds no new virtual).  The other two are the free helpers that shared the
 * translation unit with them -- the glyph drop-in animation step at
 * 0x020b060c and the sub-screen yes/no highlight repaint at 0x020b0730.
 * Both take this object's address as their first argument and are called only
 * from inside the class region, but the ROM gives neither a mangled name, so
 * neither was a class member with external linkage.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020b0580  src/_ZN13dScGameOver_cD1Ev.cpp
 *   [1] 0x020b05bc  src/_ZN13dScGameOver_cD0Ev.cpp
 *   [2] 0x020b060c  src/func_ov003_020b060c.c
 *   [3] 0x020b0730  src/func_ov003_020b0730.c
 *   [4] 0x020b0810  src/_ZN13dScGameOver_c16OnPendingDestroyEv.cpp
 *   [5] 0x020b0814  src/_ZN13dScGameOver_c6RenderEv.cpp
 *   [6] 0x020b0894  src/_ZN13dScGameOver_c8BehaviorEv.cpp
 *   [7] 0x020b0b34  src/_ZN13dScGameOver_c16CleanupResourcesEv.cpp
 *   [8] 0x020b0b3c  src/_ZN13dScGameOver_c13InitResourcesEv.cpp
 */

#include "dScGameOver_c.h"
#include "decl_Stage.h"
#include "OamAttr.h"
#include "decl_common.h"

/* CARRIED, and it has to be at file scope.  src/func_ov003_020b060c.c had
 * `#pragma opt_strength_reduction off` and tubuild create left it out with a
 * [NOT carried -- review] note, on the grounds that a file-global pragma would
 * silently recompile every other member.  It IS file-global last-wins in
 * mwccarm 2004/b56 -- bracketing it around the one member does nothing,
 * because the trailing `on` wins for the whole file -- so there is no
 * narrower place to put it.  The blast radius was measured rather than
 * assumed: with it set here, all 9 members verify, so it costs the other
 * eight nothing.  0x020b060c is the one that needs it: it walks the eight
 * glyphs with an `i << 1` index into four s16 arrays and a second `ip`
 * induction variable, which is exactly the shape mwcc strength-reduces into
 * an extra register when the pragma is absent. */
#pragma opt_strength_reduction off

/* The global object at data_0209f5bc that gates Behavior.  Only its call
 * shape is proven -- a virtual call through slot 0x14 taking itself as the
 * receiver and returning a truth value -- so the vtable is spelt out to that
 * slot and no further, and neither type has a ROM RTTI identity to name it
 * with.  Same treatment as the sub-screen OAM entry in
 * src/actors/dScMgD3DBase_c.cpp.
 *
 * RENAMED from the legacy file's `VT`/`Obj`.  `VT` is a real ROM symbol --
 * include/decl_common.h:415 declares `extern int VT[];` -- and the merge is
 * what makes both visible at once.  A variable name hides a struct tag in
 * C++, so `VT *vt;` stops naming a type and mwcc reports `undefined
 * identifier 'VT'` on the struct that uses it, never on the header.  These
 * are file-local shadow types with no ROM identity, so renaming them costs
 * nothing. */
struct GameOverGateVt { void *v0, *v1, *v2, *v3, *v4; int (*m_14)(void *); };
struct GameOverGate { GameOverGateVt *vt; };

/* ROM symbols this TU references that no header in the tree declares yet.
   Spelt by their exact final names under C linkage, the way decl_common.h
   spells its own -- a bare `void Foo(int);` from C++ would emit _Z3Fooi,
   which exists nowhere.  Everything else these functions call comes from
   decl_common.h / decl_Stage.h and is deliberately NOT repeated here.  */
extern "C" {
extern short data_ov003_020b1774[];
extern unsigned short data_ov003_020b174c[];
extern OamAttr *data_ov003_020b1824[];
extern GameOverGate *data_0209f5bc;
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];
extern int data_0208ee44;
/* RECONCILED: the legacy sources spelt these two both as `unsigned char x[]`
   (func_ov003_020b060c, which indexes [0]) and as `u8 x` (InitResources,
   which assigns and shifts the scalar) -- an array against a scalar is a hard
   conflict that tubuild's detector does not report, because it compares
   spellings and not types.  The scalar wins: it is what the rest of the tree
   uses for this pair (src/_ZN10dScEntry_c13InitResourcesEv.cpp,
   src/_ZN10dScMgCup_c13InitResourcesEv.cpp, src/_ZN11dScMgBase_c*.cpp) and
   what include/dScMgBase_c.h's prose describes -- these are the two BG-enable
   bit registers, main and sub, not tables.  func_ov003_020b060c's `[0]`
   subscripts were rewritten as plain scalar reads; the bytes are unchanged. */
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern u8 data_0209f204;
/* RESOLVED CONFLICT: func_ov003_020b0730 declared this `unsigned short *` and
   InitResources declared it `void *`.  `unsigned short *` wins -- it is the
   stronger of the two and the only one that types 0x020b0730's `p += ...`
   and 16-bit stores; InitResources only hands the result to DecompressLZ16's
   `void *` parameter, which the pointer converts to implicitly. */
extern unsigned short *_ZN3G2S12GetBG1ScrPtrEv(void);
void *_ZN2G213GetBG2CharPtrEv(void);
void *_ZN2G212GetBG2ScrPtrEv(void);
void *_ZN2G212GetBG0ScrPtrEv(void);
void *_ZN3G2S12GetBG2ScrPtrEv(void);
u32 _ZN3G2S13GetBG1CharPtrEv(void);
void _ZN2GX12SetBankForBGEt(u16 v);
void _ZN2GX13SetBankForOBJEt(u16 v);
void _ZN2GX15SetBankForSubBGEt(u16 v);
void _ZN2GX16SetBankForSubOBJEt(u16 v);
void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
s32 sub, OamAttr *data, s32 x, s32 y,
s32 palette, s32 priority, Fix12i scaleX, Fix12i scaleY,
s32 rotation, s32 mode);
void _ZN8dScene_c14StartSceneFadeEjjt(unsigned int, unsigned int, unsigned short);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);
int LoadFile(int handle);
void DecompressLZ16(int src, void *dst);
void func_02012790(int se);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 8 -- _ZN13dScGameOver_c13InitResourcesEv, 0x020b0b3c, size 0x5dc */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScGameOver_c13InitResourcesEv
/* recovered: named members + real C++ method */
/* dScGameOver_c::InitResources() -- vtable slot 0. Brings both engines up for
 * the game-over screen, loads the language-specific "GAME OVER" art (five
 * language variants on both screens), then zeroes the cursor FSM and lays the
 * eight glyphs out along unk_050/unk_060 with a fixed scale and no rotation. */
s32 dScGameOver_c::InitResources()
{
    int f;

    func_02019028();
    _ZN2GX15DisableAllBanksEv();
    _ZN2GX12SetBankForBGEt(1);
    _ZN2GX13SetBankForOBJEt(0x40);
    _ZN2GX15SetBankForSubBGEt(4);
    _ZN2GX16SetBankForSubOBJEt(8);

    *(volatile u32 *)0x4000000 &= 0xffcfffef;
    *(volatile u32 *)0x4001000 &= 0xffcfffef;
    _ZN2GX15SetGraphicsModeEiii(1, 0, 0);
    _ZN3GXS15SetGraphicsModeEi(3);

    *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x38000000) | 0x8000000;
    *(volatile u16 *)0x4000304 = (*(volatile u16 *)0x4000304 & ~0x20e) | 0x20e;
    _ZN2GX6DispOnEv();

    *(volatile u32 *)0x4001000 |= 0x10000;

    *(volatile u16 *)0x4000008 &= ~3;
    *(volatile u16 *)0x4000008 &= ~0x40;
    *(volatile u16 *)0x4000008 = (*(volatile u16 *)0x4000008 & 0x43) | 0xe10;
    SetBg0Offset(0, 0);

    *(volatile u16 *)0x400000a &= ~3;
    *(volatile u16 *)0x400000a = (*(volatile u16 *)0x400000a & 0x43) | 0x1f1c;
    *(volatile u16 *)0x400000a &= ~0x40;
    SetBg1Offset(0, 0);

    LoadDebugFont();

    *(volatile u16 *)0x400000c &= ~3;
    *(volatile u16 *)0x400000c &= ~0x40;
    *(volatile u16 *)0x400000c = (*(volatile u16 *)0x400000c & 0x43) | 0xd10;
    SetBg2Offset(0, 0);

    if (GetOwnerLanguage() == 5) {
        f = LoadFile(0xb00c);
    } else if (GetOwnerLanguage() == 4) {
        f = LoadFile(0xac0c);
    } else if (GetOwnerLanguage() == 3) {
        f = LoadFile(0xa80c);
    } else if (GetOwnerLanguage() == 2) {
        f = LoadFile(0xa40c);
    } else {
        f = LoadFile(0xa00c);
    }
    DecompressLZ16(f, _ZN2G213GetBG2CharPtrEv());
    Deallocate((void *)f);

    f = LoadFile(0x231);
    _ZN2GX10LoadBGPlttEPKvjj((const void *)f, 0, 0x1a0);
    Deallocate((void *)f);

    f = LoadFile(0x232);
    DecompressLZ16(f, _ZN2G212GetBG2ScrPtrEv());
    Deallocate((void *)f);

    f = LoadFile(0x25c);
    DecompressLZ16(f, _ZN2G212GetBG0ScrPtrEv());
    Deallocate((void *)f);

    *(volatile u16 *)0x400000e &= ~3;
    *(volatile u16 *)0x400000e = (*(volatile u16 *)0x400000e & 0x43) | 0x1100;
    *(volatile u16 *)0x400000e &= ~0x40;
    LoadFont(0);

    f = LoadFile(0x233);
    DecompressLZ16(f, (void *)0x6400000);
    Deallocate((void *)f);

    f = LoadFile(0x234);
    _ZN2GX11LoadOBJPlttEPKvjj((const void *)f, 0, 0x200);
    Deallocate((void *)f);

    data_0209d45c = 0x14;

    *(volatile u16 *)0x4001008 &= ~3;
    *(volatile u16 *)0x4001008 &= ~0x40;
    *(volatile u16 *)0x4001008 = (*(volatile u16 *)0x4001008 & 0x43) | 0x1100;
    SetSubBg0Offset(0, 0);

    *(volatile u16 *)0x400100a &= ~3;
    *(volatile u16 *)0x400100a &= ~0x40;
    *(volatile u16 *)0x400100a = (*(volatile u16 *)0x400100a & 0x43) | 0xc00;
    SetSubBg1Offset(0, 0);

    if (GetOwnerLanguage() == 5) {
        f = LoadFile(0xb00d);
    } else if (GetOwnerLanguage() == 4) {
        f = LoadFile(0xac0d);
    } else if (GetOwnerLanguage() == 3) {
        f = LoadFile(0xa80d);
    } else if (GetOwnerLanguage() == 2) {
        f = LoadFile(0xa40d);
    } else {
        f = LoadFile(0xa00d);
    }
    DecompressLZ16(f, (void *)_ZN3G2S13GetBG1CharPtrEv());
    Deallocate((void *)f);

    f = LoadFile(0x9807);
    _ZN3GXS10LoadBGPlttEPKvjj((const void *)f, 0, 0x200);
    Deallocate((void *)f);

    f = LoadFile(0x9801);
    DecompressLZ16(f, _ZN3G2S12GetBG1ScrPtrEv());
    Deallocate((void *)f);

    *(volatile u16 *)0x400100c &= ~3;
    *(volatile u16 *)0x400100c &= ~0x40;
    *(volatile u16 *)0x400100c = (*(volatile u16 *)0x400100c & 0x43) | 0xd00;
    SetSubBg2Offset(0, 0);

    f = LoadFile(0x228);
    DecompressLZ16(f, _ZN3G2S12GetBG2ScrPtrEv());
    Deallocate((void *)f);

    func_0201cebc(0x27d);
    func_ov003_020b0730(this, 1);

    unk_090 = 0;
    unk_091 = 0;
    unk_092 = 0;
    unk_093 = 0;
    unk_094 = 0;
    for (f = 0; f < 8; f++) {
        unk_050[f] = data_ov003_020b1764[f];
        unk_060[f] = 0x60;
        unk_070[f] = 0x30;
        unk_080[f] = 0;
    }

    data_0209d454 = 0x14;
    *(volatile u32 *)0x4000000 = (*(volatile u32 *)0x4000000 & ~0x1f00) | (data_0209d45c << 8);
    *(volatile u32 *)0x4001000 = (*(volatile u32 *)0x4001000 & ~0x1f00) | 0x1400;
    data_0209f204 = 1;
    _ZN5Sound16LoadInitialGroupEi(2);
    _ZN5Sound22LoadAndSetMusic_Layer1Ei(0x15);
    data_0208ee44 = 1;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- _ZN13dScGameOver_c16CleanupResourcesEv, 0x020b0b34, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScGameOver_c16CleanupResourcesEv
/* recovered: real C++ method */
/* dScGameOver_c::CleanupResources() -- vtable slot 3. The game-over scene owns
 * no allocation of its own: everything it draws is OAM data belonging to the
 * overlay, so there is nothing to release. */
s32 dScGameOver_c::CleanupResources()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN13dScGameOver_c8BehaviorEv, 0x020b0894, size 0x2a0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScGameOver_c8BehaviorEv
/* recovered: named members + real C++ method */
/* dScGameOver_c::Behavior() -- vtable slot 6. A two-state cursor FSM over the
 * yes/no prompt: state 0 reads the stylus box and the button, state 1 runs the
 * two countdowns down and commits -- continue (func_0202ae74) on "yes", or a
 * scene fade back out on "no". unk_094 gates all of it until the intro is in. */
s32 dScGameOver_c::Behavior()
{
    GameOverGate *o = data_0209f5bc;

    if (o->vt->m_14(o) == 0)
        goto end;

    func_ov003_020b060c(this);

    if (unk_094 < 2)
        return 1;

    switch (unk_090) {
        case 0: goto state0;
        case 1: goto state1;
        default: goto end;
    }

state0:
        {
            unsigned char idx = data_020a0e40;
            int b = 0;
            if (data_020a0de8[idx * 4] != 0)
                b = (data_020a0de9[idx * 4] != 0);
            if (!b) {
                if (!IsButtonInputValid())
                    goto end;
            }
        }
        {
            unsigned char idx = data_020a0e40;
            if ((unsigned char)(data_020a0dea[idx * 4] - 8) < 0xf0) {
                if ((unsigned char)(data_020a0deb[idx * 4] - 0x38) < 0x20)
                    goto left;
            }
        }
        if (unk_091 != 0)
            goto right_check;
        if (!IsButtonInputValid())
            goto right_check;
    left:
        if (unk_091 == 0)
            unk_093 = (unsigned char)(data_0208ee44 << 3);
        unk_091 = 0;
        func_ov003_020b0730(this, 0);
        unk_092 = (unsigned char)(data_0208ee44 << 4);
        func_02012790(0x9a);
        unk_090 = 1;
        goto end;

    right_check:
        {
            unsigned char idx = data_020a0e40;
            if ((unsigned char)(data_020a0dea[idx * 4] - 8) < 0xf0) {
                if ((unsigned char)(data_020a0deb[idx * 4] - 0x68) < 0x20)
                    goto right;
            }
        }
        if (unk_091 != 1)
            goto end;
        if (!IsButtonInputValid())
            goto end;
    right:
        if (unk_091 == 1)
            unk_093 = (unsigned char)(data_0208ee44 << 3);
        unk_091 = 1;
        func_ov003_020b0730(this, 0);
        unk_092 = (unsigned char)(data_0208ee44 << 4);
        func_02012790(0x9b);
        unk_090 = 1;
        goto end;

state1:
        if (unk_093 != 0) {
            unk_093 -= data_0208ee44;
            if (unk_093 == 0)
                func_ov003_020b0730(this, 0);
        }
        if (unk_092 != 0) {
            unk_092 -= data_0208ee44;
            if (unk_092 == 0) {
                if (unk_091 == 0)
                    func_0202ae74();
                else
                    _ZN8dScene_c14StartSceneFadeEjjt(1, 0, 0);
                _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
            }
        }

end:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN13dScGameOver_c6RenderEv, 0x020b0814, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScGameOver_c6RenderEv
/* recovered: named members + real C++ method */
/* dScGameOver_c::Render() -- vtable slot 9. Draws the eight "GAME OVER" glyph
 * sprites from the per-glyph x/y arrays InitResources laid out, then lets Stage
 * draw the yes/no bouncing arrows once the cursor is live. */
s32 dScGameOver_c::Render()
{
    int i;
    for (i = 0; i < 8; i++) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, data_ov003_020b1824[i],
            unk_050[i], unk_060[i],
            -1, -1, 0x1000, 0x1000, 0, -1);
    }
    if (unk_094 >= 2)
        _ZN5Stage20RenderBouncingArrowsEv();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN13dScGameOver_c16OnPendingDestroyEv, 0x020b0810, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN13dScGameOver_c16OnPendingDestroyEv
/* recovered: real C++ method */
/* dScGameOver_c::OnPendingDestroy() -- vtable slot 12. Nothing to unwind; the
 * slot exists only so dScene_c's teardown has something to call. */
void dScGameOver_c::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- func_ov003_020b0730, 0x020b0730, size 0xe0 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov003_020b0730
/* Repaints the sub-screen BG1 palette bank over the two yes/no boxes: two
 * 0x20-by-4 rectangles whose screen-map origins come from
 * data_ov003_020b174c.  `arg` non-zero forces both back to the unselected
 * bank; zero picks the highlighted bank for whichever box unk_091 names,
 * and only while the unk_093 flash timer has run out.  Free helper -- it
 * takes the receiver but carries no mangled name; declared in
 * include/decl_common.h. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov003_020b0730(void *self, int arg)
{
  unsigned char *s = (unsigned char *) self;
  int i;
  for (i = 0; i < 2; i++)
  {
    unsigned int v;
    int j;
    unsigned short *p = _ZN3G2S12GetBG1ScrPtrEv();
    p += data_ov003_020b174c[i];
    if (arg)
    {
      v = 0x1000;
    }
    else
    {
      unsigned int w;
      if ((s[0x91] == i) && (s[0x93] == 0))
        w = 0x2000;
      else
        w = 0x1000;
      v = (unsigned short) w;
    }
    for (j = 0; j < 0x20; j++)
    {
      p[0] = (p[0] & 0x3ff) | v;
      p[0x20] = (p[0x20] & 0x3ff) | v;
      p[0x40] = (p[0x40] & 0x3ff) | v;
      p[0x60] = (p[0x60] & 0x3ff) | v;
      p += 1;
    }
  }
}
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- func_ov003_020b060c, 0x020b060c, size 0x124 */
/* -------------------------------------------------------------------------- */
// @symbol func_ov003_020b060c
/* The glyph drop-in animation step, one frame per call, driven off the
 * unk_094 phase byte: phase 0 walks the eight glyphs, counting each one's
 * unk_070 delay down and then sliding its unk_050 x toward the target in
 * data_ov003_020b1774 -- the first four from the left, the last four from
 * the right -- and flips to phase 1 when the last one lands; phase 1 waits
 * out one more unk_070 tick, then reaches phase 2, which is what unblocks
 * Behavior's cursor FSM and Render's bouncing arrows, and enables the two
 * BG layers the prompt lives on.
 *
 * RECONCILED: the parameter is `void *`, not the legacy file's `char *`.
 * include/decl_common.h declares this function `extern void
 * func_ov003_020b060c(void*);` and a real header wins -- the two spellings
 * seen together are `illegal function overloading`, an error mwcc reports
 * against this definition line while saying nothing about the header.  The
 * `char *` view is kept as a local alias so the raw-offset accesses below
 * are spelt exactly as the legacy file spelt them (see the field-address
 * CSE note in notes/agents/roles/writer.md). */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov003_020b060c(void *self)
{
    char *c = (char *)self;
    unsigned char s = *(unsigned char *)(c + 0x94);
    int i;
    int ip;
    if (s == 0)
        goto case0;
    if (s == 1)
        goto case1;
    return;

case0:
    i = 0;
    ip = 0xa;
    do {
        char *e = c + (i << 1);
        if (*(unsigned short *)(e + 0x70) != 0) {
            *(unsigned short *)((long long)(int)(e + 0x70)) =
                (unsigned short)(*(unsigned short *)((long long)(int)(e + 0x70)) - 1);
        } else {
            short tgt = data_ov003_020b1774[i];
            if (*(short *)(e + 0x50) != tgt) {
                if (i < 4) {
                    short *q = (short *)(e + 0x50);
                    *q = (short)(*q + 0xc);
                    if (*(short *)(e + 0x50) >= tgt) {
                        *(short *)(e + 0x50) = tgt;
                        *(unsigned short *)(e + 0x70) = (unsigned short)ip;
                    }
                } else {
                    short *q = (short *)(e + 0x50);
                    *q = (short)(*q - 0xc);
                    if (*(short *)(e + 0x50) <= tgt) {
                        *(short *)(e + 0x50) = tgt;
                        *(unsigned short *)(e + 0x70) = (unsigned short)ip;
                        *(unsigned char *)(c + 0x94) = 1;
                    }
                }
            }
        }
        i = i + 1;
        ip = ip + 4;
    } while (i < 8);
    return;

case1:
    if (*(unsigned short *)(c + 0x70) != 0) {
        unsigned short *p = (unsigned short *)(c + 0x70);
        *p = (unsigned short)(*p - 1);
        return;
    }
    *(unsigned char *)(c + 0x94) = 2;
    data_0209d45c |= 1;
    data_0209d454 |= 3;
}
}

/* --------------------------------------------------------------------------
 * ROM ordinals 1 and 0 -- _ZN13dScGameOver_cD0Ev at 0x020b05bc (0x50)
 *                     and _ZN13dScGameOver_cD1Ev at 0x020b0580 (0x3c).
 *
 * Neither is written out here, and neither needs a forcing helper.
 *
 * ~dScGameOver_c() is defined inline in the class body
 * (include/dScGameOver_c.h) and must stay there.  Out of line, mwccarm
 * 2004/b56 emits the pair as D2, D0, D1 -- D0 above D1, the opposite of the
 * cartridge, plus a _ZN13dScGameOver_cD2Ev that exists nowhere in the ROM.
 * Inline in class is the only admissible form that gives D1 then D0, and the
 * lever set is closed: a `delete p` scaffold, a `p->~dScGameOver_c()`
 * scaffold, and moving the declaration below the overrides all change
 * nothing.  The cartridge ordered these the reproducible way -- D1 at
 * 0x020b0580 below D0 at 0x020b05bc -- so the whole TU licenses in one run.
 *
 * What emits the out-of-line pair is this TU's KEY FUNCTION.  The first
 * DECLARED non-inline virtual of the class is InitResources (the inline
 * destructor is declared first but, being inline, cannot be the key
 * function), and this TU defines it -- so mwcc emits _ZTV13dScGameOver_c
 * right here.  Slots 16 and 17 of that table name D1 and D0, which odr-uses
 * both, and the compiler emits them out of line, D1 first.  That is exactly
 * the mechanism the retail ROM used, and it is why 0x020b0580 and 0x020b05bc
 * exist at all, in that order.
 *
 * The bodies are empty and still reproduce all 0x8c bytes.  D1's three vptr
 * stores are this class's own plus dScene_c's and dBase_c's, both of which
 * are inline in class one and two levels up and so inline into this one,
 * followed by the call to fBase_c::~fBase_c; every own field is a scalar, so
 * there is no member destruction to write.  D0 adds the operator delete this
 * class inherits from its immediate base dScene_c -- mwcc finds it there and
 * inlines it, which is why nothing here mentions a heap.
 *
 * The two legacy files src/_ZN13dScGameOver_cD1Ev.cpp and
 * src/_ZN13dScGameOver_cD0Ev.cpp each carried one out-of-line definition of
 * the same destructor, which is a redefinition once they are merged.  They
 * are deleted, not ported: they were two files with no key function to lean
 * on.  Same call as src/actors/dScMgD3DBase_c.cpp made in ov006.
 * -------------------------------------------------------------------------- */
