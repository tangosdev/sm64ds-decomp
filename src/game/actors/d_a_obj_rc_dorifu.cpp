//cpp
/* ov036/daObjRc_Dorifu_c -- the Rainbow Ride drifting platform, one file.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S. mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written FIRST here and the lowest LAST. Do
 * not reorder.
 *
 * THE DESTRUCTOR IS A REAL C++ MEMBER, `virtual ~daObjRc_Dorifu_c() {}` in
 * include/daObjRc_Dorifu_c.h, defined INLINE in the class body on purpose:
 * out of line, mwccarm emits D0 before D1 (the reverse of the cartridge's
 * 0x02111f8c D1 / 0x0211200c D0) plus a homeless D2, and objisolate refuses
 * the whole TU. Defined in the class body it emits D1 then D0 and no D2, the
 * ROM's own order, and it becomes the key function: mwccarm homes
 * _ZTI/_ZTS/_ZTV16daObjRc_Dorifu_c in this TU instead of leaving them as
 * unhomed vague-linkage passengers. See daObjKm1_Dorifu_c and
 * daObjWanwanShutter_c for the same pattern on siblings of this base.
 *
 * BOTH VARIANTS ARE COMPILER-GENERATED, so nothing below defines D1 or D0 --
 * the ordinal markers just record where each one's bytes come from.
 *
 * THE BASE DESTRUCTORS ARE INLINED IN THE ROM, which is why the compiler-
 * generated bodies spell out daObjDorifu_c's and dBgActor_c's work instead of
 * calling _ZN13daObjDorifu_cD1Ev: three vptr stores (own, daObjDorifu_c's,
 * dBgActor_c's), daObjDorifu_c's dBgW_KcMbg[5] at 0x4b0 and Model[5] at 0x320
 * through __destroy_arr, then dBgActor_c's own dBgW_KcMbg at 0x124 and Model
 * at 0xd4, then dActor_c.
 *
 * Absorbed from these legacy one-function sources (ROM address order):
 *   [0] 0x02111f8c  src/_ZN16daObjRc_Dorifu_cD1Ev.cpp
 *   [1] 0x0211200c  src/_ZN16daObjRc_Dorifu_cD0Ev.cpp
 *   [2] 0x021120a0  src/_ZN16daObjRc_Dorifu_c16CleanupResourcesEv.cpp
 *   [3] 0x021120b4  src/_ZN16daObjRc_Dorifu_c13InitResourcesEv.cpp
 *   [4] 0x021120c8  src/daObjRc_Dorifu_c_classInit.cpp
 */

#include "daObjRc_Dorifu_c.h"

/* This class's own spawn-registration profile, ov036 0x02113e6c, the same
 * shape as every sibling's (see daObjRcCarpet_c's FlyingCarpetSpawnInfo and
 * daObjKm1_Dorifu_c's StairsBdwSpawnInfo): a spawn function pointer plus
 * behavior/render priority, flags, and three Fix12i range fields. `spawn`
 * here is daObjRc_Dorifu_c_classInit itself -- the ROM's own value at
 * 0x02113e6c, recovered by tools/modules.py. */
struct RcDorifuSpawnInfo {
    void *(*spawn)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 flags;
    Fix12i rangeOffsetY;
    Fix12i range;
    Fix12i drawDistance;
    u32 unk_18;
};

typedef char RcDorifuSpawnInfo_size_must_be_0x1c[
    sizeof(RcDorifuSpawnInfo) == 0x1c ? 1 : -1];

/* The resource table this platform hands to its base, five 0xc-byte entries at
 * ov036 0x02113e88, and the SharedFilePtr/CLPS_Block cells it points at --
 * both recovered by tools/modules.py from the ROM. daObjDorifuResources and
 * the two base methods that consume it are declared by include/daObjDorifu_c.h,
 * which daObjRc_Dorifu_c.h includes, so no shadow declaration is needed for it.
 *
 * tubuild's generated shadow block spelled this WRONGLY: a local
 * `struct ResourceDescriptor { void *entries[3]; }` plus raw
 * `func_ov002_020b4b6c` / `func_ov002_020b4d58` externs. Those two names are
 * exported by no module -- the addresses are real, but the symbols there are
 * _ZN13daObjDorifu_c16CleanupResourcesEP20daObjDorifuResources
 * (config/arm9/overlays/ov002/symbols.txt:196) and
 * _ZN13daObjDorifu_c13InitResourcesEP20daObjDorifuResources (:199) -- so the TU
 * would compile and byte-match and then fail the ROM link on promotion with
 * `Undefined : "func_ov002_020b4b6c"`. That is exactly what happened to
 * daObjKm3_Dorifu_c, which had the same generated block, in the same overlay,
 * inventing the same two names. */
extern "C" {
extern SharedFilePtr data_ov036_0211411c;
extern SharedFilePtr data_ov036_021140f4;
extern CLPS_Block data_ov036_02112be8;
extern SharedFilePtr data_ov036_021140fc;
extern SharedFilePtr data_ov036_021140d4;
extern CLPS_Block data_ov036_02112c08;
extern SharedFilePtr data_ov036_021140e4;
extern SharedFilePtr data_ov036_021140ec;
extern CLPS_Block data_ov036_02112a88;
extern SharedFilePtr data_ov036_02114104;
extern SharedFilePtr data_ov036_02114114;
extern CLPS_Block data_ov036_02112b88;
extern SharedFilePtr data_ov036_0211410c;
extern SharedFilePtr data_ov036_021140dc;
extern CLPS_Block data_ov036_02112ae8;
}

daObjDorifuResources data_ov036_02113e88[5] = {
    {&data_ov036_0211411c, &data_ov036_021140f4, &data_ov036_02112be8},
    {&data_ov036_021140fc, &data_ov036_021140d4, &data_ov036_02112c08},
    {&data_ov036_021140e4, &data_ov036_021140ec, &data_ov036_02112a88},
    {&data_ov036_02114104, &data_ov036_02114114, &data_ov036_02112b88},
    {&data_ov036_0211410c, &data_ov036_021140dc, &data_ov036_02112ae8}
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- daObjRc_Dorifu_c_classInit, 0x021120c8, size 0x90          */
/* -------------------------------------------------------------------------- */
extern "C" {
// @symbol daObjRc_Dorifu_c_classInit
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN10dBgActor_cC2Ev(void *);
extern void func_020733a8(void *, int, int, void *, void *);
extern int _ZTV13daObjDorifu_c[];
extern int _ZTV16daObjRc_Dorifu_c[];
extern void _ZN5ModelD1Ev(void *);
extern void _ZN5ModelC1Ev(void *);
extern void _ZN10dBgW_KcMbgD1Ev(void *);
extern void _ZN10dBgW_KcMbgC1Ev(void *);
/* Reconstructed source-style name: SM64DS proves daObjRc_Dorifu_c through
 * RTTI, allocation size, vtable identity, and the RC_DORIFU registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. The project's TrickyTriangles implementation alias remains
 * unchanged (symbols/actor_renames.tsv still carries that row). Historical
 * alias: TrickyTriangles_Spawn. */
void *daObjRc_Dorifu_c_classInit(void)
{
    char *c = (char *)_ZN7fBase_cnwEj(0xdcc);
    if (c) {
        _ZN10dBgActor_cC2Ev(c);
        *(int *)c = (int)_ZTV13daObjDorifu_c;
        func_020733a8(c + 0x320, 5, 0x50, (void *)_ZN5ModelC1Ev, (void *)_ZN5ModelD1Ev);
        func_020733a8(c + 0x4b0, 5, 0x1c8, (void *)_ZN10dBgW_KcMbgC1Ev, (void *)_ZN10dBgW_KcMbgD1Ev);
        *(int *)c = (int)&_ZTV16daObjRc_Dorifu_c[2]; /* +8: this TU defines the vtable */
    }
    return c;
}
}

extern "C" RcDorifuSpawnInfo g_profile_RC_DORIFU = {
    daObjRc_Dorifu_c_classInit,
    0x007b,
    0x00fd,
    0x00000000,
    0x00500000,
    0x01000000,
    0x02000000,
    0x00000000
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN16daObjRc_Dorifu_c13InitResourcesEv, 0x021120b4, 0x14   */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRc_Dorifu_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method */
int daObjRc_Dorifu_c::InitResources()
{
    return daObjDorifu_c::InitResources(data_ov036_02113e88);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN16daObjRc_Dorifu_c16CleanupResourcesEv, 0x021120a0, 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRc_Dorifu_c16CleanupResourcesEv
/* recovered: named members + shared header, real C++ method */
int daObjRc_Dorifu_c::CleanupResources()
{
    return daObjDorifu_c::CleanupResources(data_ov036_02113e88);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN16daObjRc_Dorifu_cD0Ev, 0x0211200c, size 0x94           */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRc_Dorifu_cD0Ev
/* recovered: real C++ deleting destructor -- the compiler emits the whole body
 *
 * D0 is the DELETING destructor: destroy through this class and its bases,
 * then return the object to its heap. Nobody writes that; declaring
 * `~daObjRc_Dorifu_c()` is enough, because mwcc emits D2, D0 and D1 together
 * and objisolate keeps the one this file is bound to. The deallocation is an
 * inline operator delete, which is why nothing here mentions a heap.
 */
/* (no separate definition: the single ~daObjRc_Dorifu_c() in
 * include/daObjRc_Dorifu_c.h emits the D0 and D1 variants together.) */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN16daObjRc_Dorifu_cD1Ev, 0x02111f8c, size 0x80           */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjRc_Dorifu_cD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * Three vtable stores and destructor calls, every one a consequence of
 * `struct daObjRc_Dorifu_c : daObjDorifu_c`: its own vptr, then
 * daObjDorifu_c's, then dBgActor_c's -- inlined, because both bases'
 * destructors are themselves inline -- then daObjDorifu_c's dBgW_KcMbg[5]
 * and Model[5] through __destroy_arr, then dBgActor_c's own dBgW_KcMbg and
 * Model, then dActor_c. This class adds no member with a destructor of its
 * own.
 */
/* (no definition here: `virtual ~daObjRc_Dorifu_c() {}` is in
 * include/daObjRc_Dorifu_c.h, and that placement is load-bearing rather than
 * stylistic -- see the class-body comment there for the emission-order
 * reasoning.) */
