/* HOST COPY of src/_ZN8Goomboss13InitResourcesEv.c -- run rel0215 wave 2, lane
 * w2-ov074. The MSVC C2561 case, the WaterBomb_State0 / Cannon_Behavior /
 * Coin_Behavior idiom.
 *
 * src/_ZN8Goomboss13InitResourcesEv.c is declared int-returning and uses
 * mwccarm's bare `return;` on its ONE early exit (the mParam == 0x1111 path).
 * mwccarm accepts that -- r0 carries whatever the last call left -- and MSVC's
 * C++ front end rejects it outright:
 *
 *   src\_ZN8Goomboss13InitResourcesEv.c(71): error C2561:
 *   '_ZN8Goomboss13InitResourcesEv': function must return a value
 *
 * It is an ERROR, not a warning, so there is no /wd to suppress and the file
 * cannot be compiled into the port as-is. It also cannot be compiled as C: the
 * TU opens a top-level `extern "C" {` block and declares `RG rg;` without the
 * struct keyword.
 *
 * THIS FILE IS THE MATCHED SOURCE WITH FIVE BODY STATEMENTS CHANGED, TWO
 * DECLARATIONS CORRECTED AND ONE BLOCK MOVED. Nothing else differs; every
 * other line is character for character the src TU. An earlier revision of
 * this header said "one statement changed", which was wrong, and a headline
 * like this one is exactly what tells the next reviewer they need not diff the
 * file -- so here is the whole list, each item argued where it is made.
 *
 * BODY STATEMENTS (5)
 *  1. line 71, the C2561 itself:
 *         src:   func_ov074_02122634(self);  return;
 *         here:  return func_ov074_02122634(self);
 *     and that is what the ROM does, not a guess: the src TU declares
 *     func_ov074_02122634 `void` (the decompiler dropped the result), but
 *     src/func_ov074_02122634.cpp -- byte-matched, complete in delinks -- is
 *     `int` and its last statement is `return 1;`. So on the ROM's own path r0
 *     holds 1 when InitResources returns, which is what this form produces.
 *  2. func_01ffa344(x) -> func_01ffa344(x, 0). See the declaration note below:
 *     the host body takes two arguments and discards the second, and
 *     port/tools/aritycheck.py's plain-name ratchet refuses a call that leaves
 *     the slot unwritten. It cannot change the result.
 *  3. MaterialChanger::Prepare's second argument, &func_021123f4  ->
 *     port_ov053_at(0x021123f4)
 *  4. MaterialChanger::SetFile's second argument, &func_021123f4  ->
 *     port_ov053_at(0x021123f4)
 *  5. TextureTransformer::SetFile's second argument, &func_021124ac ->
 *     port_ov053_at(0x021124ac)
 *     3-5 are one ruling applied three times; see the port_ov053_at note below.
 *
 * DECLARATIONS (2)
 *  6. func_ov074_02122634, `void` -> `int`, so item 1's value can be named.
 *  7. func_01ffa344, `s32 (s32)` -> `int (int, int)`, matching its host body.
 *
 * ONE BLOCK MOVED
 *  8. the twelve file-scope `extern` declarations move INSIDE the existing
 *     extern "C" block (and the two for func_021123f4 / func_021124ac are
 *     dropped, since items 3-5 no longer name them). That is the fix for
 *     twelve of the first link's LNK2019s; see the note on the block itself.
 *
 * The //cpp marker line is also absent, because this file is already .cpp.
 *
 * The matched source stays byte-locked in src/ as proof and is dropped from
 * port/slice_ov074.txt. Delete this and put it back in the slice the moment
 * that bare return is reconciled upstream.
 *
 * PORT_HOST_ABI: MSVC C2561 rejects the matched int-with-bare-return form.
 */
#include "types.h"
struct SharedFilePtr;
struct BMD_File;
struct BCA_File;
struct BTP_File;
struct BMA_File;
struct BTA_File;
struct Vector3;
struct Actor;
struct Vector3_16;

extern "C" {
    int func_ov074_02122634(char *self);   /* src says void; the body returns 1 */
    void *_ZN5Model8LoadFileER13SharedFilePtr(void *shared);
    void LoadKeyModels(int idx);
    void *_ZN9Animation8LoadFileER13SharedFilePtr(void *shared);
    void *_ZN15TextureSequence8LoadFileER13SharedFilePtr(void *shared);
    void _ZN9ModelBase7SetFileEP8BMD_Fileii(void *mb, void *bmd, int a, int b);
    void _ZN11ShadowModel12InitCylinderEv(void *self);
/* THE ITCM f2i PRIMITIVE, DECLARED AS ITS HOST BODY DEFINES IT.
   The ROM function at 0x01ffa344 takes ONE argument (a float bit pattern in
   r0) and the src TU declares it that way. Its host body in
   hal/cxx_aliases.cpp:154 is `int func_01ffa344(int a, int b)` with `(void)b`
   -- the two-argument form exists for ONE other caller
   (src/func_ov002_020dc560.c) whose own declaration is an r1 ride-through --
   and under cdecl a one-argument call leaves that second slot unwritten.
   The body discards it, so nothing reads it today, but
   port/tools/aritycheck.py's plain-name ratchet is right to refuse the shape:
   declare the full signature and pass it. The explicit 0 is the whole fix and
   it cannot change the result. */
    int func_01ffa344(int a, int b);
    void _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
        void *self, void *actor, void *pos, s32 fx, s32 fy, u32 a, u32 b);
    void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int a, s32 fix, u32 c);
    void _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(void *bmd, void *btp);
    void _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(void *bmd, void *bma);
    void _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(void *bmd, void *bta);
    void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *self, void *f, int a, s32 fix, u32 c);
    void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(void *self, void *f, int a, s32 fix, u32 c);
    void _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(void *self, void *f, int a, s32 fix, u32 c);
    void _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(
        void *self, void *actor, s32 fa, s32 fb, void *v0, void *v1);
    void _ZN13RaycastGroundC1Ev(void *self);
    void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *self, void *pos, void *actor);
    int _ZN13RaycastGround10DetectClsnEv(void *self);
    void _ZN13RaycastGroundD1Ev(void *self);
    void func_ov074_02121300(void *c);
    void func_ov074_0212195c(void *t);

/* THE MOUNT AND ROMDATA NAMES, MOVED INSIDE extern "C".
   In the src TU these fourteen sit at file scope in a //cpp file, so MSVC
   decorates each with its declared type (?data_ov074_02123000@@3PAXA and
   friends) while the mount, romdata and the ov084 mount all emit the one C
   name -- the ov025/ov058 C++-MANGLED SPELLING class, fourteen LNK2019s off
   the first link. Here they are inside the block, which is the fix rather
   than fourteen /alternatename bridges. Nothing else about them changes. */
    extern void *data_ov002_0210da30;
    extern void *data_ov084_02130cf8;
    extern void *data_ov074_0212292c[];
    extern void *data_ov074_02122948[];
    extern void *data_ov074_02123040;
    extern void *data_ov074_02123000;
    extern void *data_ov074_02122e4c[];
    extern void *data_ov074_02123030;
    extern void *data_ov074_02122e5c[];
    extern u16 data_ov074_02122e04[];
    extern u16 data_ov074_02122dfc[];
    extern s16 data_02082214[];

/* THE TWO LEVEL-OVERLAY FILE OBJECTS, RESOLVED BY ADDRESS.
   The src TU spells them `func_021123f4` and `func_021124ac` and takes their
   ADDRESS. Both are ov053's own data -- config/arm9/overlays/ov053/symbols.txt
   names them data_ov053_021123f4 and data_ov053_021124ac, both `ambiguous`
   because every level overlay shares the 0x021111a0 window, and level 45's
   own overlay IS ov053 -- so their bytes live inside ov053's WHOLE-image
   mount, which provides no C name for any interior address. port_ov053_at()
   is the mount's own DS-to-host resolver (the JRB STATIC_ROCK precedent,
   hal/level_boot.cpp:1781) and it is a pure function over a static array, so
   it is valid before the level is entered. Taking `&func_021124ac` instead
   would have handed MaterialChanger and TextureTransformer a RAW DS ADDRESS:
   ov053's image lives at a host address, not at 0x021111a0, so the read would
   have landed in the port's zeroed reserved DS pages -- no fault, no link
   error, a garbage file header. */
    void *port_ov053_at(unsigned ds);   /* build/port/host-src/ov053_data.c */
}

struct RG { char pad[0x4c]; };

// PORT_HOST_ABI: MSVC C2561 rejects the matched int-with-bare-return form.
extern "C" int _ZN8Goomboss13InitResourcesEv(char *self)
{
    s32 i;
    s32 j;
    s32 k;
    char *c5;
    char *c8;
    void *p;
    RG rg;
    s32 v[3];

    if (*(u32 *)(self + 8) == 0x1111) {
        return func_ov074_02122634(self);
    }

    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov002_0210da30);
    LoadKeyModels(2);
    _ZN5Model8LoadFileER13SharedFilePtr(&data_ov084_02130cf8);

    i = 0;
    do {
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov074_0212292c[i]);
        i++;
    } while (i < 7);

    i = 0;
    do {
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov074_02122948[i]);
        i++;
    } while (i < 0xc);

    _ZN15TextureSequence8LoadFileER13SharedFilePtr(&data_ov074_02123040);
    p = _ZN5Model8LoadFileER13SharedFilePtr(&data_ov074_02123000);
    _ZN9ModelBase7SetFileEP8BMD_Fileii(self + 0x210, p, 1, -1);

    j = 0;
    c5 = self + 0x274;
    do {
        _ZN11ShadowModel12InitCylinderEv(c5);
        j++;
        c5 += 0x28;
    } while (j < 3);

    *(u8 *)(self + 0x604) = 3;
    p = (void *)func_01ffa344((s32)data_ov074_02122e4c[*(u8 *)(self + 0x604)], 0);
    *(void **)(self + 0x80) = p;
    *(void **)(self + 0x84) = p;
    *(void **)(self + 0x88) = p;
    *(s32 *)(self + 0x9c) = -0xa000;
    *(s32 *)(self + 0xa0) = -0x3c000;

    *(s32 *)(self + 0x5d0) = *(s32 *)(self + 0x5c);
    *(s32 *)(self + 0x5d4) = *(s32 *)(self + 0x60);
    *(s32 *)(self + 0x5d8) = *(s32 *)(self + 0x64);

    c8 = self + 0x110;
    k = 0;
    do {
        _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(
            c8, self, self + 0x5c, *(s32 *)(self + 0x80) * 0xa, *(s32 *)(self + 0x84) * 0xa, 0x200004, 0x26fe0);
        k++;
        c8 += 0x40;
    } while (k < 4);

    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x210, *((void **)&data_ov074_02123030 + 1), 0, 0x1000, 0);
    _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File(*((void **)&data_ov074_02123000 + 1), *((void **)&data_ov074_02123040 + 1));
    _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(*((void **)&data_ov074_02123000 + 1), port_ov053_at(0x021123f4));

    i = 0;
    do {
        _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File(*((void **)&data_ov074_02123000 + 1), data_ov074_02122e5c[i]);
        i++;
    } while (i < 7);

    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(self + 0x3e4, *((void **)&data_ov074_02123040 + 1), 0, 0x1000, 0);
    _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(self + 0x3d0, port_ov053_at(0x021123f4), 0x40000000, 0x1000, 0);
    _ZN18TextureTransformer7SetFileER8BTA_Filei5Fix12IiEj(self + 0x3f8, port_ov053_at(0x021124ac), 0, 0x1000, 0);

    *(s32 *)(self + 0x3f0) = 0;
    *(s32 *)(self + 0x3d8) = (s32)(data_ov074_02122e04[*(u8 *)(self + 0x604)] << 0xc);

    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(self + 0x40c, self, 0x14000, 0x14000, 0, 0);

    *(s32 *)(self + 0x5f0) = -1;
    *(s16 *)(self + 0x5f8) = (s16)(*(s32 *)(self + 0x5f0) * data_ov074_02122dfc[*(u8 *)(self + 0x604)]);
    *(s16 *)(self + 0x5f6) = 0;
    *(s16 *)(self + 0x5f4) = 0x1000;
    {
        s16 *pf = (s16 *)(((s32)self + 0x5f4) & 0xFFFFFFFFFFFFFFFFull);
        *pf += *(s16 *)(self + 0x5f6);
    }

    *(s32 *)(self + 0x5c) = (s32)(((s64)data_02082214[(*(u16 *)(self + 0x5f4) >> 4) * 2] * 0x546000 + 0x800) >> 0xc);
    *(s32 *)(self + 0x64) = (s32)(((s64)data_02082214[(*(u16 *)(self + 0x5f4) >> 4) * 2 + 1] * 0x546000 + 0x800) >> 0xc);

    {
        s32 a2;
        s32 a1;
        s32 a0;
        s32 a3;
        a2 = *(s32 *)(self + 0x60);
        a1 = *(s32 *)(self + 0x64);
        a0 = *(s32 *)(self + 0x5c);
        a3 = a2 + 0x64000;
        v[0] = a0;
        v[1] = a3;
        v[2] = a1;
    }

    _ZN13RaycastGroundC1Ev(&rg);
    _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rg, v, 0);
    if (_ZN13RaycastGround10DetectClsnEv(&rg) != 0) {
        *(s32 *)(self + 0x60) = *(s32 *)((char *)&rg + 0x44);
    }

    func_ov074_02121300(self);
    func_ov074_0212195c(self);

    *(s32 *)(self + 0x5dc) = 0;
    *(u8 *)(self + 0x60a) = 1;
    *(s32 *)(self + 0x5e4) = 0x1000;

    _ZN13RaycastGroundD1Ev(&rg);

    return 1;
}
