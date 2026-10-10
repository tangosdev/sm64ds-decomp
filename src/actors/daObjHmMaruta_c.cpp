//cpp
/**
 * daObjHmMaruta_c -- Tall, Tall Mountain's rolling log (HM_MARUTA).
 *
 * The whole class is three forwarders. daObjMaruta_c leaves InitResources,
 * CleanupResources and Behavior pure; each leaf fills them by handing
 * `this` and its own per-stage table to the shared ov080 bodies in
 * src/game/actors/d_a_obj_maruta.cpp. ov022's daObjFlMaruta_c is the other
 * leaf and does the same with its own tables.
 *
 * NAME: daObjHmMaruta_c is the cartridge's RTTI spelling -- _ZTS at ov030
 * 0x02115a10 is the byte string "15daObjHmMaruta_c", _ZTI at 0x021159f8
 * reads [__si_class_type_info+8, that string, _ZTI13daObjMaruta_c (ov022)],
 * and the typeinfo word of the vtable storage at 0x02115a40 points at that
 * _ZTI. The tree's earlier spelling, RollingLogTtm, was coined.
 *
 * THIS TU IS THE CLASS'S KEY-FUNCTION TU, so it also emits
 * _ZTV15daObjHmMaruta_c, _ZTI15daObjHmMaruta_c and _ZTS15daObjHmMaruta_c as
 * vague linkage, with daObjMaruta_c's, dBgActor_c's, dActor_c's, dBase_c's
 * and fBase_c's RTTI records. Every one has a configured ROM home, so all
 * thirteen license as deadstrip-data.
 *
 * THE DESTRUCTOR is declared and defined inline and empty in
 * include/daObjHmMaruta_c.h, which puts the key function on Behavior and
 * makes the vtable's slots 16 and 17 emit D1 (0x0211155c) then D0
 * (0x021115ac) -- the cartridge order -- with no D2. There is deliberately
 * no destructor text in this .cpp for an @symbol marker to sit above; both
 * variants score through that inline header definition.
 *
 * Written in reverse ROM order: mwccarm emits one .text section per
 * function, last-defined first. daObjHmMaruta_c_classInit (historical alias
 * RollingLogTtm_Spawn, folded in from fold-lane-c-0929, 0x0211164c..
 * 0x02111688) is highest and so comes first in source, ahead of
 * InitResources.
 *
 * Leftover: the ov080 helpers keep their linker names and C linkage, and the
 * two per-stage tables keep data_ov030_* names.
 */

#include "daObjHmMaruta_c.h"
#include "SharedFilePtr.h"

/* The model handle constructs through func_02017acc and destroys through
 * func_02017ab4. The collision handle constructs through func_02017b4c and
 * destroys through SharedFilePtr_Destruct_Clsn. The descriptor stays ROM data. */
struct MarutaModelFilePtr : SharedFilePtr {
    unsigned int words[2];
    MarutaModelFilePtr(unsigned int fileId);
    ~MarutaModelFilePtr();
};
struct MarutaClsnFileHandle : SharedFilePtr {
    unsigned int words[2];
    MarutaClsnFileHandle(unsigned int fileId);
    ~MarutaClsnFileHandle();
};
typedef char MarutaModelFilePtr_size_must_be_8[sizeof(MarutaModelFilePtr) == 8 ? 1 : -1];
typedef char MarutaClsnFileHandle_size_must_be_8[sizeof(MarutaClsnFileHandle) == 8 ? 1 : -1];

struct CLPS_Block;

/* The model/collision/CLPS triple func_ov080_021274ac and _021270dc take,
 * spelt as src/game/actors/d_a_obj_maruta.cpp spells it. ov030's copy is
 * data_ov030_02115a04: its three words relocate to 0x02115ca8 and 0x02115cb0,
 * this overlay's model and collision SharedFilePtrs, and to the CLPS block
 * at 0x02114f24. */
struct ResourceDescriptor {
    SharedFilePtr *model;
    SharedFilePtr *collision;
    CLPS_Block *clps;
};

extern "C" {
/* The helpers live in ov080, out of branch range of this overlay, so the ROM
 * reaches them with the pooled `ldr ip,[pc]; bx ip` tail call. long_calls is
 * positional and binds at the declaration; closed again straight after. */
#pragma long_calls on
int func_ov080_021270dc(daObjMaruta_c *self, ResourceDescriptor *arg);
int func_ov080_0212714c(daObjMaruta_c *self, int *maxDist);
int func_ov080_021274ac(daObjMaruta_c *self, ResourceDescriptor *arg);
#pragma long_calls off

extern ResourceDescriptor data_ov030_02115a04;
extern int data_ov030_021159f4;
}

/* Reconstructed source-style name: SM64DS proves daObjHmMaruta_c through
 * RTTI, allocation size, vtable identity, and the HM_MARUTA registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: RollingLogTtm_Spawn.
 *
 * Written first in source (this TU has no #pragma defer_codegen, so mwccarm
 * emits one .text section per function, last-defined first): this factory
 * is the highest ROM address (0x0211164c), so it must be first here for the
 * three explicitly-written functions below to keep coming out in their
 * documented ROM order. */
// @symbol daObjHmMaruta_c_classInit
extern "C" daObjHmMaruta_c *daObjHmMaruta_c_classInit()
{
    return new daObjHmMaruta_c();
}

// @symbol _ZN15daObjHmMaruta_c13InitResourcesEv
/* Vtable slot 0. */
int daObjHmMaruta_c::InitResources()
{
    return func_ov080_021274ac(this, &data_ov030_02115a04);
}

// @symbol _ZN15daObjHmMaruta_c8BehaviorEv
/* Vtable slot 6. The word handed over reads 0x412000 on the cartridge. */
int daObjHmMaruta_c::Behavior()
{
    return func_ov080_0212714c(this, &data_ov030_021159f4);
}

// @symbol _ZN15daObjHmMaruta_c16CleanupResourcesEv
/* Vtable slot 3. */
int daObjHmMaruta_c::CleanupResources()
{
    return func_ov080_021270dc(this, &data_ov030_02115a04);
}

/* Retail construction order: model 1562, then collision 1563.
 * mwcc emits __sinit_daObjHmMaruta_c.cpp. */
MarutaModelFilePtr data_ov030_02115ca8(1562);
MarutaClsnFileHandle data_ov030_02115cb0(1563);
