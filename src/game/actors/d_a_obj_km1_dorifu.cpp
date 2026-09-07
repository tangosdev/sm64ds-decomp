//cpp
/* Manually curated production translation unit.
 * ov043/daObjKm1_Dorifu_c  (5 function(s))
 *
 * One intact mwccarm object owns the five retail text contributions together
 * with this class's RTTI, type-name string, SpawnInfo, resource table, and
 * vtable storage.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in reverse source order. The destructor
 * variants are the documented compiler-chosen exception.
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x02111630  src/_ZN17daObjKm1_Dorifu_cD1Ev.cpp
 *   [1] 0x021116b0  src/_ZN17daObjKm1_Dorifu_cD0Ev.cpp
 *   [2] 0x02111744  src/_ZN17daObjKm1_Dorifu_c16CleanupResourcesEv.cpp
 *   [3] 0x02111758  src/_ZN17daObjKm1_Dorifu_c13InitResourcesEv.cpp
 *   [4] 0x0211176c  src/StairsBdw_Spawn.cpp
 */

#include "daObjKm1_Dorifu_c.h"

struct StairsBdwSpawnInfo {
    daObjKm1_Dorifu_c *(*spawn)();
    s16 behaviorPriority;
    s16 renderPriority;
    u32 flags;
    Fix12i rangeOffsetY;
    Fix12i range;
    Fix12i drawDistance;
    u32 unk_18;
};

typedef char StairsBdwSpawnInfo_size_must_be_0x1c[
    sizeof(StairsBdwSpawnInfo) == 0x1c ? 1 : -1];

extern "C" {
extern void *_ZN7fBase_cnwEj(unsigned size);
extern void _ZN10dBgActor_cC2Ev(void *self);
extern void func_020733a8(void *base, int count, int stride,
                          void *constructor, void *destructor);
extern int _ZTV13daObjDorifu_c[];
extern int _ZTV17daObjKm1_Dorifu_c[];
extern void _ZN5ModelC1Ev(void *self);
extern void _ZN5ModelD1Ev(void *self);
extern void _ZN10dBgW_KcMbgC1Ev(void *self);
extern void _ZN10dBgW_KcMbgD1Ev(void *self);

extern SharedFilePtr data_ov043_021126a0;
extern SharedFilePtr data_ov043_02112678;
extern CLPS_Block data_ov043_02111c40;
extern SharedFilePtr data_ov043_02112680;
extern SharedFilePtr data_ov043_02112658;
extern CLPS_Block data_ov043_02111b60;
extern SharedFilePtr data_ov043_02112668;
extern SharedFilePtr data_ov043_02112670;
extern CLPS_Block data_ov043_02111ba0;
extern SharedFilePtr data_ov043_02112688;
extern SharedFilePtr data_ov043_02112698;
extern CLPS_Block data_ov043_02111c80;
extern SharedFilePtr data_ov043_02112690;
extern SharedFilePtr data_ov043_02112660;
extern CLPS_Block data_ov043_02111c60;
}

/* ROM ordinal 4 -- daObjKm1_Dorifu_c_classInit, 0x0211176c, size 0x90 */
// @symbol daObjKm1_Dorifu_c_classInit
/* Reconstructed source-style name: SM64DS proves daObjKm1_Dorifu_c through RTTI,
 * allocation size, vtable identity, and the KM1_DORIFU registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: StairsBdw_Spawn. */
extern "C" daObjKm1_Dorifu_c *daObjKm1_Dorifu_c_classInit()
{
    char *actor = static_cast<char *>(_ZN7fBase_cnwEj(0xdcc));

    if (actor) {
        _ZN10dBgActor_cC2Ev(actor);
        *reinterpret_cast<int *>(actor) = (int)_ZTV13daObjDorifu_c;
        func_020733a8(actor + 0x320, 5, 0x50,
                      (void *)_ZN5ModelC1Ev, (void *)_ZN5ModelD1Ev);
        func_020733a8(actor + 0x4b0, 5, 0x1c8,
                      (void *)_ZN10dBgW_KcMbgC1Ev,
                      (void *)_ZN10dBgW_KcMbgD1Ev);
        *reinterpret_cast<int *>(actor) =
            (int)&_ZTV17daObjKm1_Dorifu_c[2];
    }

    return reinterpret_cast<daObjKm1_Dorifu_c *>(actor);
}

/* Reconstructed source-style name: SM64DS proves this descriptor through
 * its registry role, the KM1_DORIFU literal ROM profile ID, and the
 * factory relocation it carries; later EAD lineage supplies g_profile_.
 * Exact original spelling is not preserved. Historical alias:
 * StairsBdw_SpawnInfo. */
extern "C" StairsBdwSpawnInfo g_profile_KM1_DORIFU = {
    daObjKm1_Dorifu_c_classInit,
    0x0086,
    0x00af,
    0,
    0x00500000,
    0x01000000,
    0x02000000,
    0
};

extern "C" daObjDorifuResources data_ov043_02112518[5] = {
    {&data_ov043_021126a0, &data_ov043_02112678, &data_ov043_02111c40},
    {&data_ov043_02112680, &data_ov043_02112658, &data_ov043_02111b60},
    {&data_ov043_02112668, &data_ov043_02112670, &data_ov043_02111ba0},
    {&data_ov043_02112688, &data_ov043_02112698, &data_ov043_02111c80},
    {&data_ov043_02112690, &data_ov043_02112660, &data_ov043_02111c60}
};

/* ROM ordinal 3 -- _ZN17daObjKm1_Dorifu_c13InitResourcesEv */
// @symbol _ZN17daObjKm1_Dorifu_c13InitResourcesEv
int daObjKm1_Dorifu_c::InitResources()
{
    return daObjDorifu_c::InitResources(data_ov043_02112518);
}

/* ROM ordinal 2 -- _ZN17daObjKm1_Dorifu_c16CleanupResourcesEv */
// @symbol _ZN17daObjKm1_Dorifu_c16CleanupResourcesEv
int daObjKm1_Dorifu_c::CleanupResources()
{
    return daObjDorifu_c::CleanupResources(data_ov043_02112518);
}
