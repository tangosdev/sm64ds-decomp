//cpp
/* dScTitle_c -- the title / level-select screen, ov003.
 *
 * The screen the game boots into: a 0x36-cell level grid with a cursor the
 * D-pad walks by row (+0x35) or column (+1), a confirm that either fades to
 * the picked level or hands off to one of the two special entries, and a
 * scrolling window that keeps the cursor on screen. 9 functions,
 * .text 0x020ad660..0x020adc74: the seven dScene_c slots every direct child
 * overrides (0, 3, 6, 9, 12, 16, 17 -- the class adds no new virtual), the
 * one free helper that shares the TU, and the factory dScTitle_c_classInit,
 * which abuts InitResources at 0x020adc10. D1/D0 are not written here: the
 * header's inline destructor plus the key function (InitResources) emits
 * them, D1 first, the ROM's order.
 *
 * Source order is the reverse of the ROM's -- mwccarm emits .text in
 * reverse source order. Do not reorder.
 *
 * deslop leftovers:
 * - func_ov003_020ad6ec is written free here, and that is a reconstruction
 *   choice, not a deduction. The image preserves no original linker symbol
 *   table, so `func_ov003_*` is an address-derived analysis label, and RTTI
 *   supplies class identities, not function spellings
 *   (notes/tu-promotion-conventions.md section 1 and
 *   notes/symbol-name-provenance.md). Both call sites hand it this object
 *   and every call to it is inside this class's region: that is evidence to
 *   narrow ownership on later -- call, layout or codegen -- not proof that
 *   the original was free. Its parameter is UNUSED by the body, while both
 *   call sites do load the receiver into r0, so the parameter is kept and
 *   spelled `void *` -- the one spelling neither call site has to cast away
 *   from. Until the ownership is narrowed, the original form stays
 *   uncertain, and the free form is only what this TU reproduces.
 * - OAM::EnableSubOAM stays mangled with a local one-int shadow parameter
 *   type: the ROM name carries by-value class parameters that mwccarm
 *   passes differently at the call site, so declaring the true types breaks
 *   the byte match (notes/mwccarm-codegen.md 6az). Carried verbatim from
 *   the legacy Render source, unchanged.
 * - data_0209f5bc (the active scene) gates Behavior through a plain virtual
 *   call on slot 5; the tree's spellings disagree with each other and none
 *   has a ROM RTTI identity, so the TU-local shadow spells six virtuals and
 *   Behavior calls v5. Same slot as d_s_mg_base.cpp's SceneVCall6 and
 *   src/actors/dScGameOver_c.cpp's SceneGate.
 * - The GX/G2 shadow namespaces have no header in the tree yet; they are
 *   carried from the legacy InitResources source unchanged.
 * - unk_050: the one own field, a repeat-delay counter. See dScTitle_c.h.
 */

#include "dScTitle_c.h"
#include "decl_common.h"
#include "Sound.h"

/* The active scene at data_0209f5bc, which gates Behavior. Only its call
 * shape is proven -- a virtual call through slot 5 taking itself as the
 * receiver and returning a truth value -- so the class spells six virtuals
 * and Behavior calls v5. It has no ROM RTTI identity to name it with. */
struct VObj {
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual int v5();
};

/* OAM::EnableSubOAM's receiver. A one-int stand-in carried verbatim from
 * the legacy Render source: the point of the local type is that the true
 * by-value class parameter is what mwccarm passes differently. */
struct OamTmp { int x; };

namespace OAM { void EnableSubOAM(OamTmp*);
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
/* local extern: the matched Render passes &tmp in r0; the header's
   static EnableSubOAM() takes no argument */
extern "C" void _ZN3OAM12EnableSubOAMEv(OamTmp*);
 }

/* No header in the tree declares this namespace yet. Carried from the
 * legacy InitResources source unchanged. Its sibling `namespace G2` could
 * NOT be carried: decl_common.h:395 declares a real ROM symbol
 * `extern int G2[];`, and a variable-hides-a-tag lookup turns the namespace
 * into `illegal namespace` / `undefined identifier 'G2'` the moment this TU
 * includes that header. G2::GetBG0ScrPtr is therefore spelt by its final
 * mangled name in the block below, the way the rest of the tree spells it. */
namespace GX {
    void DisableAllBanks();
    void SetBankForBG(unsigned short);
    void SetBankForOBJ(unsigned short);
    void SetGraphicsMode(int, int, int);
    void LoadBG0Char(void const *, unsigned int, unsigned int);
    void LoadBGPltt(void const *, unsigned int, unsigned int);
    void LoadOBJ(void const *, unsigned int, unsigned int);
    void LoadOBJPltt(void const *, unsigned int, unsigned int);
}

/* ROM symbols this TU references that no header in the tree declares yet,
 * spelt by their exact final names under C linkage the way decl_common.h
 * spells its own. Kept above the first `// @symbol` marker so none of these
 * mangled spellings is charged to a member
 * (notes/tu-promotion-conventions.md section 6). Everything else these
 * functions call comes from decl_common.h or Sound.h and is deliberately
 * NOT repeated here. */
extern "C" {
/* The level-grid cursor index. RECONCILED: the three legacy sources spelt
 * it `int[]`, `int` and `u32`. `u32` is the one the bytes need, because
 * Behavior's `% 0x36` is an unsigned reduction; the array spelling was only
 * ever read at [0]. */
extern u32 data_0209b2f4;
extern char data_0209e674[];
extern VObj *data_0209f5bc;
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u16 data_020a0e58[];
extern u8 data_0209f2d8;
extern u16 data_0209f5e8[];
extern int data_020a0d90[];
extern u8 data_0209d45c[];
extern u8 data_0208ee44[];
extern u8 data_0209ee90[];
extern void func_02012790(int idx);
extern void _ZN5Sound22StopLoadedMusic_Layer1Ej(u32 a);
extern u16 DecIfAbove0_Short(u16 *p);
extern void Enable3dEngines(void);
extern void func_ov003_020adcbc(int a, int b, int c);
extern void func_ov003_020adc74(void *p, int b, int c);
/* `void *` is the tree's plurality spelling for this one (it is declared
 * five different ways across five files); the cast at the one call site is
 * free. */
void *_ZN2G212GetBG0ScrPtrEv(void);
/* RECONCILED: the two legacy call sites spelt the parameter `int` and
 * `char *`; the body ignores it. `void *` is the spelling neither call site
 * has to cast away from. */
extern void func_ov003_020ad6ec(void *self);
}

/* Reconstructed source-style name: SM64DS proves dScTitle_c through RTTI,
 * allocation size, vtable identity, and the TITLE registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * Historical alias: func_ov003_020adc10.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x020adc10 loads 0x54 -- the class's own size -- into fBase_c::operator
 * new; the inlined ctor runs fBase_c's C2, stores dBase_c then dScene_c's
 * vptrs, ORs pauseFlags 1 and 4 (dScene_c::dScene_c), then stores this
 * class's vptr. The null check is the one `new` itself emits. */
// @symbol dScTitle_c_classInit
extern "C" dScTitle_c *dScTitle_c_classInit(void)
{
    return new dScTitle_c();
}

/* [7] 0x020ada9c -- vtable slot 0, KEY FUNCTION. Brings the 2D engines up
 * for the title screen: banks, graphics mode, BG0 char/palette and the OBJ
 * set, blanks the BG0 screen map, then seeds the scroll window from the
 * cursor and starts the title music. */
// @symbol _ZN10dScTitle_c13InitResourcesEv
s32 dScTitle_c::InitResources()
{
    UnloadArchives();
    Enable3dEngines();
    GX::DisableAllBanks();
    GX::SetBankForBG(8);
    GX::SetBankForOBJ(1);
    *(volatile u32*)0x4000000 &= 0xffcfffef;
    GX::SetGraphicsMode(1, 0, 0);
    *(volatile u32*)0x4001000 |= 0x10000;
    func_020233f4();
    *(volatile u16*)0x4000008 &= ~3;
    *(volatile u16*)0x4000008 = (*(volatile u16*)0x4000008 & 0x43) | 0x1f00;
    *(volatile u16*)0x4000008 &= ~0x40;
    GX::LoadBG0Char(data_0208a178, 0, 0x2000);
    GX::LoadBGPltt(data_0208c178, 0, 0x200);
    {
        u16 *scr = (u16 *)_ZN2G212GetBG0ScrPtrEv();
        int i = 0;
        do { *scr++ = 0xff; i++; } while (i < 0x1800);
    }
    GX::LoadOBJ(data_0208c378, 0, 0x2000);
    GX::LoadOBJPltt(data_0208c178, 0, 0x200);
    *(u8*)data_0209d45c = 0x11;
    *(volatile u32*)0x4000000 = (*(volatile u32*)0x4000000 & ~0x1f00) | 0x1100;
    *(int*)data_0208ee44 = 2;
    func_ov003_020ad6ec(this);
    *(volatile u32*)0x4000010 = 0;
    *(u8*)(data_0209ee90 + 0x340) = 0;
    *(u8*)(data_0209ee90 + 0x341) = 0;
    Sound::LoadInitialGroup(2);
    Sound::LoadAndSetMusic_Layer1(0x38);
    return 1;
}

/* [6] 0x020ada80 -- vtable slot 3. The title scene owns only its voice
 * group; releasing that is the whole teardown. */
// @symbol _ZN10dScTitle_c16CleanupResourcesEv
s32 dScTitle_c::CleanupResources()
{
    Sound::UnsetPlayerVoiceGroup();
    return 1;
}

/* [5] 0x020ad814 -- vtable slot 6. Level-select cursor update: on confirm
 * (or minigame-active flag) starts the scene fade / loads the picked level
 * from the 8-byte entry table at data_ov003_020b1180; otherwise moves the
 * cursor by row (+0x35) or column (+1) with repeat delay at unk_050,
 * wrapping the index modulo 0x36. */
// @symbol _ZN10dScTitle_c8BehaviorEv
s32 dScTitle_c::Behavior()
{
    if (data_0209f5bc->v5()) {
        int r3 = 0;
        u8 idx = data_020a0e40;
        if (data_020a0de8[idx * 4] != 0)
            r3 = data_020a0de9[idx * 4] != 0;
        if (((data_020a0e58[1] & 9) | r3) == 0)
            goto buttons;
        {
            u32 i;
            s8 t;
            int f;
            func_02012790(1);
            i = data_0209b2f4;
            t = data_ov003_020b1180[i * 8];
            if (t == -1)
                goto fade5;
            f = 0;
            {
                u8 j = data_020a0e40;
                if (data_020a0de8[j * 4] != 0)
                    f = data_020a0de9[j * 4] != 0;
            }
            if (f == 0)
                goto chk;
        fade5:
            dScene_c::StartSceneFade(5, 0, 0);
            _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
            goto tail;
        chk:
            if (t == -2) {
                dScene_c::StartSceneFade(6, 0, 0);
                _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
            } else {
                data_0209f2d8 = 0;
                LoadLevelNoReturn(t, data_ov003_020b1181[i << 3], 1, 0);
                SetPlayerGlobals();
                SetNumPlayers(1);
                dScene_c::StartSceneFade(4, 0, 0);
                data_0209f5e8[6] = 0x7fff;
            }
            goto tail;
        }
    }
    {
    buttons:
        u16 v = data_020a0e58[0];
        if (v & 0x40) {
            int h = data_020a0e58[1] & 0x40;
            if (h == 0) {
                if (unk_050 != 0)
                    goto tail;
            }
            unk_050 = h ? 8 : 2;
            data_0209b2f4 += 0x35;
            func_02012790(0);
        } else if (v & 0x80) {
            int h = data_020a0e58[1] & 0x80;
            if (h == 0) {
                if (unk_050 != 0)
                    goto tail;
            }
            unk_050 = h ? 8 : 2;
            data_0209b2f4 += 1;
            func_02012790(0);
        }
    }

tail:
    data_0209b2f4 = data_0209b2f4 % 0x36;
    DecIfAbove0_Short(&unk_050);
    func_ov003_020ad6ec(this);
    return 1;
}

/* [4] 0x020ad7a4 -- vtable slot 9. Real C++ method over the shared header;
 * this body never touches its own fields. */
// @symbol _ZN10dScTitle_c6RenderEv
s32 dScTitle_c::Render()
{
    OamTmp tmp;
    OAM::_ZN3OAM12EnableSubOAMEv(&tmp);
    {
        int idx = data_0209e664;
        unsigned int v = ((data_0209b2f4 - data_0209b2f8 + 2) << 3) & 0xff;
        char* p = data_0209e674 + (idx << 3);
        *(int*)p = v | 0x80000;
        *(short*)(p + 4) = 0;
    }
    return 1;
}

/* [3] 0x020ad7a0 -- vtable slot 12. Nothing to unwind; the slot exists only
 * so dScene_c's teardown has something to call. */
// @symbol _ZN10dScTitle_c16OnPendingDestroyEv
void dScTitle_c::OnPendingDestroy()
{
}

/* [2] 0x020ad6ec -- scrolls the level-grid window so the cursor stays on
 * screen: clamps the origin at data_0209b2f8, redraws the visible rows
 * through func_ov003_020adcbc, then repaints the caption. Written free here
 * as a reconstruction choice; see the file header. The parameter is unused
 * by the body, but both call sites load the receiver into r0.
 *
 * RECONCILED: decl_common.h already declares data_0209b2f8 `extern int`, so
 * the legacy `int[]` spelling is dropped in favour of the header's scalar
 * and the `[0]` subscripts became scalar accesses. */
// @symbol func_ov003_020ad6ec
extern "C" {  /* .c-derived member: C linkage for the whole block */
void func_ov003_020ad6ec(void *)
{
    int v = data_0209b2f4;
    int nv = v - 0xa;
    data_0209b2f8 = nv;
    unsigned int t = (unsigned int)(v + 0xb);
    if (t > 0x36) {
        data_0209b2f8 = nv - (t - 0x36);
    }
    if (data_0209b2f8 < 0) data_0209b2f8 = 0;
    int s = data_0209b2f8;
    int cnt = 0x36 - s;
    int end = cnt + 2;
    if (end > 0x17) end = 0x17;
    int i;
    int j = s;
    for (i = 2; i < end; i++) {
        func_ov003_020adcbc(j, i, 3);
        j++;
    }
    func_ov003_020adc74(data_020a0d90, 0, 0);
}
}

/* [1] [0] 0x020ad69c (D0) and 0x020ad660 (D1) -- neither is written here.
 * The header's inline destructor is what emits the pair, D1 first, the
 * ROM's order (out of line mwccarm emits D2, D0, D1 plus a D2 the ROM never
 * carried). The key function is InitResources -- the first DECLARED
 * non-inline virtual -- and defining it here emits the vtable whose slots
 * 16/17 odr-use both halves. The empty bodies still reproduce all 0x8c
 * bytes: three inlined vptr stores plus fBase_c::~fBase_c, and D0's
 * inherited operator delete. See include/dScTitle_c.h. */
