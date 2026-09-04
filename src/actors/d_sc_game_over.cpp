//cpp
/* ov003 / dScGameOver_c -- the GAME OVER screen scene (registry profile
 * GAME_OVER, RTTI name at ov003:0x020b1784).
 *
 * One translation unit for the whole ROM run 0x020b0580..0x020b117c: the five
 * class methods, the destructor pair, the two file-local helpers between them,
 * and the factory, plus the .data run 0x020b174c..0x020b17e4 that holds the
 * class's two layout tables, its profile descriptor, its RTTI chain and its
 * vtable.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order.
 *
 * .data OBJECTS ARE NOT LAID OUT IN SOURCE ORDER: mwld sorts a TU's .data
 * contributions by ASCENDING SIZE (measured across the three other ov003
 * scene TUs), which is what puts the 4-byte table first, the 8-byte profile
 * next, _ZTI (0xc) after it, the two 0x10 tables and _ZTS next, and the 0x50
 * vtable object last. Source order only breaks the 0x10 tie, so the two s16
 * tables are written in their ROM order.
 *
 * The destructor is NOT written out of line below. It is defined in the class
 * body in include/dScGameOver_c.h, which is what makes mwccarm emit the
 * cartridge's D1/D0 pair, in the cartridge's order, and no D2 at all.
 *
 * `#pragma opt_strength_reduction off` came from the legacy func_ov003_020b060c
 * source. It is file-global last-wins in this compiler, so it is set once here
 * and every member of the TU was re-verified under it.
 */
#pragma opt_strength_reduction off

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "dScGameOver_c.h"
#include "decl_Stage.h"
#include "OamAttr.h"
#include "decl_common.h"

/* Local shadow declarations carried from the legacy files verbatim.
 * NOT reconciled against real project headers -- check include/*.h for
 * each of these before compiling; a real header should usually win. */
/* Local shadow of the global at data_0209f5bc and the one slot Behavior calls
 * through it. Renamed off the generator's `VT`/`Obj`: decl_common.h already
 * has a data symbol spelled VT. */
struct GameOverVT { void *v0, *v1, *v2, *v3, *v4; int (*m_14)(void *); };
struct GameOverObj { GameOverVT *vt; };

extern "C" {
extern unsigned short *_ZN3G2S12GetBG1ScrPtrEv(void);
extern OamAttr *data_ov003_020b1824[];
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(
s32 sub, OamAttr *data, s32 x, s32 y,
s32 palette, s32 priority, Fix12i scaleX, Fix12i scaleY,
s32 rotation, s32 mode);
extern GameOverObj *data_0209f5bc;
int IsButtonInputValid(void);
void func_02012790(int se);
void func_0202ae74(void);
void _ZN8dScene_c14StartSceneFadeEjjt(unsigned int, unsigned int, unsigned short);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];
extern int data_0208ee44;
void _ZN2GX12SetBankForBGEt(u16 v);
void _ZN2GX13SetBankForOBJEt(u16 v);
void _ZN2GX15SetBankForSubBGEt(u16 v);
void _ZN2GX16SetBankForSubOBJEt(u16 v);
int LoadFile(int handle);
void *_ZN2G213GetBG2CharPtrEv(void);
void *_ZN2G212GetBG2ScrPtrEv(void);
void *_ZN2G212GetBG0ScrPtrEv(void);
void *_ZN3G2S12GetBG2ScrPtrEv(void);
u32 _ZN3G2S13GetBG1CharPtrEv(void);
void DecompressLZ16(int src, void *dst);
void _ZN2GX10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN2GX11LoadOBJPlttEPKvjj(const void *p, u32 a, u32 b);
void _ZN3GXS10LoadBGPlttEPKvjj(const void *p, u32 a, u32 b);
extern u8 data_0209d45c;
extern u8 data_0209d454;
extern u8 data_0209f204;

/* Factory-side imports. */
extern void *data_0208e4b8;
void *_ZN7fBase_cnwEj(unsigned int);
void _ZN7fBase_cC2Ev(void *);
/* G2S::GetBG1ScrPtr was spelled `void *` by the InitResources source and
 * `unsigned short *` by the func_ov003_020b0730 source; the typed spelling
 * wins and the void* use site converts implicitly. */
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 9 -- dScGameOver_c_classInit, 0x020b1118, size 0x64            */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves dScGameOver_c through RTTI,
 * allocation size, vtable identity, and the GAME_OVER registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: func_ov003_020b1118.
 *
 * The three vptr stores are the constructor chain flattened by inlining:
 * fBase_c's, dScene_c's, then this class's. */
// @symbol dScGameOver_c_classInit
extern "C" void *dScGameOver_c_classInit(void)
{
    char *p = (char *)_ZN7fBase_cnwEj(0x98);
    if (p) {
        _ZN7fBase_cC2Ev(p);
        *(void **)p = &data_0208e4b8;
        *(void **)p = (void *)_ZTV8dScene_c;
        *(unsigned char *)(int)(((int)p + 0x13)) |= 1;
        *(unsigned char *)(int)(((int)p + 0x13)) |= 4;
        /* &[2], not the bare symbol: this TU EMITS the vtable, so mwcc's
         * symbol is the object start at 0x020b1794 and +8 is what reaches the
         * 0x020b179c address point the ROM stores. The addend-0 spelling is
         * right only while a class merely IMPORTS its vtable. */
        *(void **)p = (void *)&_ZTV13dScGameOver_c[2];
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* The TU's own .data (see the size-ordering note in the file header)          */
/* -------------------------------------------------------------------------- */
/* data_ov003_020b174c, 0x020b174c, size 0x4 -- the two BG1 tilemap offsets
 * func_ov003_020b0730 highlights, one per yes/no box. */
// @symbol data_ov003_020b174c
extern "C" unsigned short data_ov003_020b174c[2] = { 0x00e0, 0x01a0 };

struct GameOverSpawnInfo {
    void *(*classInit)();
    s16 profileIDAndExecuteOrder;
    s16 drawOrder;
};
typedef char GameOverSpawnInfo_size_must_be_0x8[
    sizeof(GameOverSpawnInfo) == 0x8 ? 1 : -1];

/* g_profile_GAME_OVER, 0x020b1750, size 0x8 -- a scene profile, so it carries
 * only the factory and the two order words; scenes have no clip volume. */
// @symbol g_profile_GAME_OVER
extern "C" GameOverSpawnInfo g_profile_GAME_OVER = {
    dScGameOver_c_classInit, 0x0008, 0x000b
};

/* data_ov003_020b1764, 0x020b1764, size 0x10 -- the eight glyphs' start x,
 * read straight into unk_050 by InitResources. */
// @symbol data_ov003_020b1764
extern "C" short data_ov003_020b1764[8] = {
    -96, -69, -42, -16, 274, 297, 325, 352
};

/* data_ov003_020b1774, 0x020b1774, size 0x10 -- the same eight glyphs' resting
 * x, which func_ov003_020b060c slides them to. */
// @symbol data_ov003_020b1774
extern "C" short data_ov003_020b1774[8] = {
    32, 59, 86, 112, 146, 169, 197, 224
};

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
    GameOverObj *o = data_0209f5bc;

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

/* -------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one inline definition, two ROM-visible variants:   */
/*   _ZN13dScGameOver_cD1Ev  0x020b0580  size 0x3c  (complete-object dtor)    */
/*   _ZN13dScGameOver_cD0Ev  0x020b05bc  size 0x50  (deleting destructor)     */
/* -------------------------------------------------------------------------- */
/* Defined in include/dScGameOver_c.h's class body, and deliberately NOT
 * repeated out of line here: out of line mwccarm emits D2, D0, D1, and the
 * cartridge holds D1, D0 with no D2 at all. The body is the three vptr stores
 * and the base subobject teardown -- this class adds no member with a
 * destructor of its own. D0 additionally returns the object to its heap
 * through the inline operator delete it inherits from dScene_c, which is why
 * nothing here mentions a heap. */

// @symbol _ZN13dScGameOver_cD0Ev
// @symbol _ZN13dScGameOver_cD1Ev
