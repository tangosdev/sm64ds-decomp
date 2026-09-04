//cpp
/* ov045 / daObjKm2_Ukishima_c -- Bowser in the Fire Sea's floating floor
 * (registry profile KM2_UKISHIMA, RTTI name at ov045:0x02112f14).
 *
 * One translation unit for the whole ROM run 0x02111b14..0x02111c30: the two
 * overrides, the destructor pair and the factory, plus the .data run
 * 0x02112efc..0x02112fd0 that holds the class's RTTI pair, the three-pointer
 * file table its two overrides pass on, the profile descriptor and the
 * vtable.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S.
 * mwccarm 2004/b56 emits one `.text` section per function and lays those
 * sections out in the object in the REVERSE of their order in the source text,
 * so writing the LAST function of the ROM run FIRST is what makes the object's
 * section order equal the ROM's address order.
 *
 * .data objects are laid out by mwld in ASCENDING SIZE: the two 0xc objects
 * first, then _ZTS (0x16), the profile (0x1c) and the 0x88 vtable object --
 * the ROM's own order for this run.
 *
 * The destructor is NOT written out of line below. It is defined in the class
 * body in include/daObjKm2_Ukishima_c.h, which is what makes mwccarm emit the
 * cartridge's D1/D0 pair, in the cartridge's order, and no D2 at all.
 */

/* Includes: union of the legacy files', first-seen in ROM-ascending
 * processing order. NOT verified for header ordering constraints (e.g. a
 * common.h-before-X rule) -- watch for new compile errors after this. */
#include "daObjKm2_Ukishima_c.h"

extern "C" {
extern int func_ov002_020b6424(void *self, void *filetable);
extern int func_ov002_020b6584(void *self, void *filetable, int angle);

/* The three objects the file table below points at: two BSS SharedFilePtrs
 * and the CLPS block in .data. */
extern u8 data_ov045_02113200[];
extern u8 data_ov045_021131f8[];
extern u8 data_ov045_02112550[];

/* Factory-side imports. daObjUkiyuka_c's vtable is only IMPORTED here, so it
 * keeps the addend-0 spelling. */
void *_ZN7fBase_cnwEj(unsigned int);
void _ZN10dBgActor_cC2Ev(void *);
extern int _ZTV14daObjUkiyuka_c[];
extern int _ZTV19daObjKm2_Ukishima_c[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- daObjKm2_Ukishima_c_classInit, 0x02111bf4, size 0x3c      */
/* -------------------------------------------------------------------------- */
/* Reconstructed source-style name: SM64DS proves daObjKm2_Ukishima_c through
 * RTTI, allocation size, vtable identity, and the KM2_UKISHIMA registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: FloatingFloorBfs_Spawn.
 *
 * The two vptr stores are the constructor chain flattened by inlining:
 * daObjUkiyuka_c's, then this class's. */
// @symbol daObjKm2_Ukishima_c_classInit
extern "C" int *daObjKm2_Ukishima_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(812);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV14daObjUkiyuka_c;
        /* &[2], not the bare symbol: this TU EMITS its own vtable, so mwcc's
         * symbol is the object start at 0x02112f48 and +8 is what reaches the
         * 0x02112f50 address point the ROM stores. */
        p[0] = (int)&_ZTV19daObjKm2_Ukishima_c[2];
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* The TU's own .data (see the size-ordering note in the file header)          */
/* -------------------------------------------------------------------------- */
/* data_ov045_02112f08, 0x02112f08, size 0xc -- the file table both overrides
 * hand to daObjUkiyuka_c's shared helpers: two SharedFilePtrs in BSS and the
 * CLPS block at 0x02112550. Spelled int[] to match decl_common.h. */
// @symbol data_ov045_02112f08
extern "C" int data_ov045_02112f08[3] = {
    (int)data_ov045_02113200, (int)data_ov045_021131f8, (int)data_ov045_02112550
};

/* g_profile_KM2_UKISHIMA, 0x02112f2c, size 0x1c */
struct UkishimaSpawnInfo {
    int *(*classInit)();
    s16 profileIDAndExecuteOrder;   /* 0x0091 */
    s16 drawOrder;                  /* 0x00d8 */
    u32 actorFlags;
    s32 clipOffsetY;
    s32 clipRadius;
    s32 clipDistance;
    s32 farDistance;
};
typedef char UkishimaSpawnInfo_size_must_be_0x1c[
    sizeof(UkishimaSpawnInfo) == 0x1c ? 1 : -1];

// @symbol g_profile_KM2_UKISHIMA
extern "C" UkishimaSpawnInfo g_profile_KM2_UKISHIMA = {
    daObjKm2_Ukishima_c_classInit, 0x0091, 0x00d8, 0x00000002,
    0x00060000, 0x01000000, 0x02000000, 0x00000000
};

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN19daObjKm2_Ukishima_c13InitResourcesEv, 0x02111bdc, size 0x18 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjKm2_Ukishima_c13InitResourcesEv
/* daObjKm2_Ukishima_c::InitResources -- vtable slot 0, ov045 0x02111bdc.
 *
 * A tail call into daObjUkiyuka_c's shared cross-overlay file-load helper
 * func_ov002_020b6584(this, filetable, angle) -- out of this task's
 * scope, kept under its existing name, same cross-overlay-veneer idiom as
 * the other abstract-base leaves in this worklist (ov015/ov022/ov036's
 * daObjKaitendai_c leaves, ov043's daObjKurumajiku_c leaf). */
s32 daObjKm2_Ukishima_c::InitResources()
{
    return func_ov002_020b6584(this, data_ov045_02112f08, 0xf50);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN19daObjKm2_Ukishima_c16CleanupResourcesEv, 0x02111bc8, size 0x14 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19daObjKm2_Ukishima_c16CleanupResourcesEv
/* daObjKm2_Ukishima_c::CleanupResources -- vtable slot 3, ov045 0x02111bc8.
 *
 * A tail call into daObjUkiyuka_c's shared cleanup helper
 * func_ov002_020b6424(this, filetable) -- out of this task's scope, kept
 * under its existing name. */
s32 daObjKm2_Ukishima_c::CleanupResources()
{
    return func_ov002_020b6424(this, data_ov045_02112f08);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinals 1 and 0 -- one inline definition, two ROM-visible variants:   */
/*   _ZN19daObjKm2_Ukishima_cD1Ev  0x02111b14  size 0x50                      */
/*   _ZN19daObjKm2_Ukishima_cD0Ev  0x02111b64  size 0x64                      */
/* -------------------------------------------------------------------------- */
/* Defined in include/daObjKm2_Ukishima_c.h's class body, and deliberately NOT
 * repeated out of line here: out of line mwccarm emits D2, D0, D1, and the
 * cartridge holds D1, D0 with no D2 at all.
 *
 * D1 makes THREE vptr stores, and that is the shape of the chain: its own,
 * daObjUkiyuka_c's (inlined -- that destructor is defined in its class body),
 * then dBgActor_c's, then dBgActor_c's dBgW_KcMbg and Model, then dActor_c.
 * Nothing in the chain adds a member with a destructor of its own. */

// @symbol _ZN19daObjKm2_Ukishima_cD0Ev
// @symbol _ZN19daObjKm2_Ukishima_cD1Ev
