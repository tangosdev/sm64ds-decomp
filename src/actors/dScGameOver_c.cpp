//cpp
/* dScGameOver_c -- the game-over screen, ov003.
 *
 * Mario is out of lives: eight "GAME OVER" glyphs drop in from off screen,
 * then a yes/no continue prompt (stylus or button; left is yes). 9
 * functions, .text 0x020b0580..0x020b1118: the seven dScene_c slots every
 * direct child overrides (0, 3, 6, 9, 12, 16, 17 -- the class adds no new
 * virtual) plus the two free helpers that share the TU. D1/D0 are not
 * written here: the header's inline destructor plus the key function
 * (InitResources) emits them, D1 first, the ROM's order.
 *
 * Source order is the reverse of the ROM's -- mwccarm emits .text in
 * reverse source order. Do not reorder.
 *
 * deslop leftovers:
 * - The two func_ov003_* helpers are written free here, and that is a
 *   reconstruction choice, not a deduction. The image preserves no original
 *   linker symbol table, so `func_ov003_*` are address-derived analysis
 *   labels; their absence from the reconstructed mangled set is evidence of
 *   nothing about the original spelling. Both take the object and are called
 *   only from inside the class region -- worth narrowing later on call,
 *   layout or codegen grounds. Until then the original ownership and form
 *   stay uncertain; the free form is what this TU reproduces.
 * - `#pragma opt_strength_reduction off` is file-global last-wins;
 *   func_ov003_020b060c needs it (glyph-loop induction) and the other eight
 *   members verify with it set, so it costs nothing. No narrower form.
 * - OAM::Render's Fix12 overload stays mangled: OAM.h excludes the by-value
 *   Fix12<int> overloads (codegen wall). The G2/GX/Sound decls below have no
 *   header; _ZN3G2S12GetBG1ScrPtrEv keeps the u16* spelling for 0730's
 *   pointer arithmetic (typing ergonomics, not a codegen wall -- void*
 *   compiles identically, see notes/experiments/gameover-2711-bg1scrptr-u16.md).
 * - data_0209f5bc (the active scene) gates Behavior through a plain virtual
 *   call on slot 5; the tree's spellings disagree with each other
 *   (SceneVCall6, UnkObj, ...) and none has a ROM RTTI identity.
 * - unk_080[8]: zeroed by InitResources, never read back.
 */

#include "dScGameOver_c.h"
#include "Stage.h"
#include "OamAttr.h"
#include "decl_common.h"

/* File scope is the only place this pragma works. 0x020b060c needs
 * `#pragma opt_strength_reduction off`: it walks the eight glyphs with an
 * `i << 1` index into s16 arrays plus a second `ip` induction variable,
 * which is exactly the shape mwcc strength-reduces into an extra register
 * when the pragma is absent. The pragma IS file-global last-wins in mwccarm
 * 2004/b56 -- bracketing it around the one member does nothing, because the
 * trailing `on` wins for the whole file -- so there is no narrower place to
 * put it. The blast radius was measured rather than assumed: with it set
 * here, all 9 members verify, so it costs the other eight nothing. */
#pragma opt_strength_reduction off

/* The active scene at data_0209f5bc, which gates Behavior. Only its call
 * shape is proven -- a virtual call through slot 5 taking itself as the
 * receiver and returning a truth value -- so the class spells six virtuals
 * and Behavior calls f05. It has no ROM RTTI identity to name it with. Same
 * slot as d_s_mg_base.cpp's SceneVCall6. */
struct SceneGate { virtual int f00(); virtual int f01(); virtual int f02(); virtual int f03(); virtual int f04(); virtual int f05(); };

/* ROM symbols this TU references that no header in the tree declares yet.
 * Spelt by their exact final names under C linkage, the way decl_common.h
 * spells its own. Everything else these functions call comes from Stage.h,
 * OamAttr.h, or decl_common.h and is deliberately NOT repeated here. */
extern "C" {
extern short data_ov003_020b1774[];
extern unsigned short data_ov003_020b174c[];
extern OamAttr *data_ov003_020b1824[];
extern SceneGate *data_0209f5bc;
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];
extern int data_0208ee44;
/* Scalars, not tables: the two BG-enable bit registers, main and sub --
 * the spelling the rest of the tree uses for this pair. */
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern u8 data_0209f204;
/* u16* spelling for 0730's `p += ...` and 16-bit stores, no cast needed.
 * Ergonomics, not a constraint: the void* form compiles to identical bytes
 * at both consumers (see notes/experiments/gameover-2711-bg1scrptr-u16.md).
 * Init passes the result to DecompressLZ16's void* param either way. */
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
void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);
int LoadFile(int handle);
void DecompressLZ16(int src, void *dst);
void func_02012790(int se);
}

/* [8] 0x020b0b3c -- vtable slot 0, KEY FUNCTION */
 // @symbol _ZN13dScGameOver_c13InitResourcesEv
/* Brings both engines up for the game-over screen, loads the
 * language-specific "GAME OVER" art (five language variants on both
 * screens), then zeroes the cursor FSM and lays the eight glyphs out along
 * mGlyphX/mGlyphY with a fixed scale and no rotation. */
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

    mCursorState = 0;
    mSelection = 0;
    mCommitTimer = 0;
    mFlashTimer = 0;
    mIntroPhase = 0;
    for (f = 0; f < 8; f++) {
        mGlyphX[f] = data_ov003_020b1764[f];
        mGlyphY[f] = 0x60;
        mGlyphDelay[f] = 0x30;
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

/* [7] 0x020b0b34 -- vtable slot 3. Owns no allocation: everything drawn is
 * OAM data belonging to the overlay, so there is nothing to release. */
 // @symbol _ZN13dScGameOver_c16CleanupResourcesEv
s32 dScGameOver_c::CleanupResources()
{
    return 1;
}

/* [6] 0x020b0894 -- vtable slot 6. A two-state cursor FSM over the yes/no
 * prompt: state 0 reads the stylus box and the button, state 1 runs the two
 * countdowns down and commits -- continue (func_0202ae74) on "yes", or a
 * scene fade back out on "no". mIntroPhase gates all of it until the intro
 * is in. The goto skeleton is the ROM's own branch shape. */
 // @symbol _ZN13dScGameOver_c8BehaviorEv
s32 dScGameOver_c::Behavior()
{
    if (data_0209f5bc->f05() == 0)
        goto end;

    func_ov003_020b060c(this);

    if (mIntroPhase < 2)
        return 1;

    switch (mCursorState) {
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
        if (mSelection != 0)
            goto right_check;
        if (!IsButtonInputValid())
            goto right_check;
    left:
        if (mSelection == 0)
            mFlashTimer = (unsigned char)(data_0208ee44 << 3);
        mSelection = 0;
        func_ov003_020b0730(this, 0);
        mCommitTimer = (unsigned char)(data_0208ee44 << 4);
        func_02012790(0x9a);
        mCursorState = 1;
        goto end;

    right_check:
        {
            unsigned char idx = data_020a0e40;
            if ((unsigned char)(data_020a0dea[idx * 4] - 8) < 0xf0) {
                if ((unsigned char)(data_020a0deb[idx * 4] - 0x68) < 0x20)
                    goto right;
            }
        }
        if (mSelection != 1)
            goto end;
        if (!IsButtonInputValid())
            goto end;
    right:
        if (mSelection == 1)
            mFlashTimer = (unsigned char)(data_0208ee44 << 3);
        mSelection = 1;
        func_ov003_020b0730(this, 0);
        mCommitTimer = (unsigned char)(data_0208ee44 << 4);
        func_02012790(0x9b);
        mCursorState = 1;
        goto end;

state1:
        if (mFlashTimer != 0) {
            mFlashTimer -= data_0208ee44;
            if (mFlashTimer == 0)
                func_ov003_020b0730(this, 0);
        }
        if (mCommitTimer != 0) {
            mCommitTimer -= data_0208ee44;
            if (mCommitTimer == 0) {
                if (mSelection == 0)
                    func_0202ae74();
                else
                    dScene_c::StartSceneFade(1, 0, 0);
                _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
            }
        }

end:
    return 1;
}

/* [5] 0x020b0814 -- vtable slot 9. Draws the eight "GAME OVER" glyph
 * sprites from the per-glyph x/y arrays InitResources laid out, then lets
 * Stage draw the yes/no bouncing arrows once the cursor is live. */
 // @symbol _ZN13dScGameOver_c6RenderEv
s32 dScGameOver_c::Render()
{
    int i;
    for (i = 0; i < 8; i++) {
        _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
            0, data_ov003_020b1824[i],
            mGlyphX[i], mGlyphY[i],
            -1, -1, 0x1000, 0x1000, 0, -1);
    }
    if (mIntroPhase >= 2)
        Stage::RenderBouncingArrows();
    return 1;
}

/* [4] 0x020b0810 -- vtable slot 12. Nothing to unwind; the slot exists only
 * so dScene_c's teardown has something to call. */
 // @symbol _ZN13dScGameOver_c16OnPendingDestroyEv
void dScGameOver_c::OnPendingDestroy()
{
}

/* [3] 0x020b0730 -- repaints the sub-screen BG1 palette bank over the two
 * yes/no boxes: two 0x20-by-4 rectangles whose screen-map origins come from
 * data_ov003_020b174c. `arg` non-zero forces both back to the unselected
 * bank; zero picks the highlighted bank for whichever box mSelection names,
 * and only while the mFlashTimer has run out. Free helper -- it takes the
 * receiver but carries no mangled name. */
 // @symbol func_ov003_020b0730
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov003_020b0730(void *self, int arg)
{
  dScGameOver_c *o = (dScGameOver_c *)self;
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
      if ((o->mSelection == i) && (o->mFlashTimer == 0))
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

/* [2] 0x020b060c -- the glyph drop-in step, one frame per call. */
 // @symbol func_ov003_020b060c
/* offsetof without <stddef.h>: the tree includes no system headers, and
 * func_ov003_020b060c is the one member that needs field offsets as
 * constants. dScGameOver_c's layout is pinned by the header's sizeof
 * assert, so these cannot drift silently. */
#define GO_OFF(m) ((int)&((dScGameOver_c *)0)->m)

/* The glyph drop-in animation step, one frame per call, driven off
 * mIntroPhase: phase 0 walks the eight glyphs, counting each one's
 * mGlyphDelay down and then sliding its mGlyphX toward the target in
 * data_ov003_020b1774 -- the first four from the left, the last four from
 * the right -- and flips to phase 1 when the last one lands; phase 1 waits
 * out one more mGlyphDelay tick, then reaches phase 2, which is what unblocks
 * Behavior's cursor FSM and Render's bouncing arrows, and enables the two
 * BG layers the prompt lives on.
 *
 * The `e` loop skeleton is load-bearing, not leftover. MEASURED: spelling
 * the loop with member indexing (o->mGlyphX[i] / o->mGlyphDelay[i]) costs 3
 * words -- the ROM computes e = base + (i << 1) once per iteration and
 * reaches both rows as small constant offsets off e, while the member-index
 * form makes mwccarm compute two separate base+i*2 addresses. GO_OFF names
 * those offsets after their fields without changing the tree the compiler
 * sees. The goto/case skeleton, the `short *q` re-reads, the (long long)(int)
 * launders on the decrement, and case1's `(char *)o + GO_OFF` spelling (the
 * member-index form perturbs the tail) are the ROM's own shape too. */
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov003_020b060c(void *self)
{
    dScGameOver_c *o = (dScGameOver_c *)self;
    unsigned char s = o->mIntroPhase;
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
        char *e = (char *)o + (i << 1);
        if (*(unsigned short *)(e + GO_OFF(mGlyphDelay)) != 0) {
            *(unsigned short *)((long long)(int)(e + GO_OFF(mGlyphDelay))) =
                (unsigned short)(*(unsigned short *)((long long)(int)(e + GO_OFF(mGlyphDelay))) - 1);
        } else {
            short tgt = data_ov003_020b1774[i];
            if (*(short *)(e + GO_OFF(mGlyphX)) != tgt) {
                if (i < 4) {
                    short *q = (short *)(e + GO_OFF(mGlyphX));
                    *q = (short)(*q + 0xc);
                    if (*(short *)(e + GO_OFF(mGlyphX)) >= tgt) {
                        *(short *)(e + GO_OFF(mGlyphX)) = tgt;
                        *(unsigned short *)(e + GO_OFF(mGlyphDelay)) = (unsigned short)ip;
                    }
                } else {
                    short *q = (short *)(e + GO_OFF(mGlyphX));
                    *q = (short)(*q - 0xc);
                    if (*(short *)(e + GO_OFF(mGlyphX)) <= tgt) {
                        *(short *)(e + GO_OFF(mGlyphX)) = tgt;
                        *(unsigned short *)(e + GO_OFF(mGlyphDelay)) = (unsigned short)ip;
                        o->mIntroPhase = 1;
                    }
                }
            }
        }
        i = i + 1;
        ip = ip + 4;
    } while (i < 8);
    return;

case1:
    if (*(unsigned short *)((char *)o + GO_OFF(mGlyphDelay)) != 0) {
        unsigned short *p = (unsigned short *)((char *)o + GO_OFF(mGlyphDelay));
        *p = (unsigned short)(*p - 1);
        return;
    }
    o->mIntroPhase = 2;
    data_0209d45c |= 1;
    data_0209d454 |= 3;
}
}

/* [1] [0] 0x020b05bc (D0) and 0x020b0580 (D1) -- neither is written here.
 * The header's inline destructor is what emits the pair, D1 first, the ROM's
 * order (out of line mwccarm emits D2, D0, D1 plus a D2 the ROM never
 * carried). The key function is InitResources -- the first DECLARED
 * non-inline virtual -- and defining it here emits the vtable whose slots
 * 16/17 odr-use both halves. The empty bodies still reproduce all 0x8c
 * bytes: three inlined vptr stores plus fBase_c::~fBase_c, and D0's
 * inherited operator delete. See include/dScGameOver_c.h. */
