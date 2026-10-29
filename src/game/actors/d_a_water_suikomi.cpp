//cpp
/* Production translation unit for ov026/daWater_Suikomi_c.
 * 13 function(s), .text 0x021121fc..0x02112490. The WATER_SUIKOMI actor, the
 * water suction in Wet-Dry World.
 *
 * NAME: _ZTS17daWater_Suikomi_c is "17daWater_Suikomi_c" at ov026 0x02113dec;
 * _ZTI at 0x02113de0 reads [__si_class_type_info, that string,
 * _ZTI12dEnemyBase_c (ov002 0x021081c0)], and the word before the
 * _ZTV17daWater_Suikomi_c address point (0x02113e24) points at that _ZTI.
 * Slots 16/17 of that vtable are D1 (0x021121fc) and D0 (0x02112234). The
 * tree previously called the class WaterSuction (coined; that spelling is not
 * in the cartridge).
 *
 * This is the whole unit: the destructor pair, the five non-virtual members
 * between D0 and CleanupResources, the five virtuals, then the factory.
 * InitState0 (0x021122cc) and State0 (0x021122b0) are the two member-function
 * pointers that ov026's last static initializer (0x02112d68) copies from
 * 0x02113dd8/0x02113dd0 into the state record at 0x02113f58; SetState
 * (0x021122d4) installs a state record at +0x30c (InitResources hands it
 * 0x02113f58); and Behavior calls the empty func_ov026_02112324. (A vtable scan
 * that runs past daWater_Tatumaki_c's 31 slots at 0x02113d54 reads those two
 * member-function-pointer constants as Tatumaki "slots 31 and 33"; they are
 * not Tatumaki's, and no Tatumaki function lies in this range.)
 *
 * The out-of-line destructor is the key function, so this TU emits
 * _ZTV/_ZTI/_ZTS17daWater_Suikomi_c; the manifest's compiler_only_output rows
 * route those (and the homeless D2) to the cartridge's own copies. The
 * registry factory daWater_Suikomi_c_classInit (0x02112450) closes the unit
 * and ov026's .text.
 *
 * Under `#pragma defer_codegen off` .text is laid down in source order, so
 * this file is ROM-ascending.
 *
 * What stays:
 * - The empty member at 0x02112324 keeps its address name: Behavior calls it
 *   where sibling actors rebuild their model matrix, but its body says
 *   nothing about its role.
 */
#include "daWater_Suikomi_c.h"
#include "math/CalcLib.h"

/* The ROM member-function-pointer constants the state table copies from
 * (unlicensed .data). */
extern daWater_Suikomi_c::StateFn data_ov026_02113dd8;
extern daWater_Suikomi_c::StateFn data_ov026_02113dd0;

/* The state table at 0x02113f58: the one state this TU's InitResources
 * hands to SetState, plus section-end fill (see the definition at the end of
 * this file). */
struct SuikomiStateTable {
    daWater_Suikomi_c::State state;
    u8 tailFill[0x18];
};
extern SuikomiStateTable data_ov026_02113f58;

/* The collider offset. A plain int triple, not Vector3: a Vector3 local
 * would emit Vector3's inline destructor, which this TU's manifest does not
 * license. */
struct V3 {
    int x, y, z;
};
extern V3 data_ov026_02113f4c;

#pragma defer_codegen off

// @symbol _ZN17daWater_Suikomi_cD1Ev
// @symbol _ZN17daWater_Suikomi_cD0Ev
/* The key function. D1 stores the vtable, destroys mWithMeshClsn (+0x150)
 * and mdCcAcPos_c (+0x110) in reverse declaration order, then runs
 * dEnemyBase_c's D2; D0 does the same and hands the object to the inline
 * operator delete. The base-object D2 mwcc also emits has no home in the
 * cartridge. */
daWater_Suikomi_c::~daWater_Suikomi_c() {}

// @symbol _ZN17daWater_Suikomi_c12CheckContactEv
/* Looks up the actor the collider last touched (mdCcAcPos_c.otherOwner) and
 * does nothing with it: the ROM body returns either way. */
void daWater_Suikomi_c::CheckContact() {
    unsigned int id = mdCcAcPos_c.otherOwner;
    if (id == 0)
        return;
    dActor_c *other = dActor_c::FindWithID(id);
    if (other == 0)
        return;
}

// @symbol _ZN17daWater_Suikomi_c6State0Ev
/* The state's per-frame function (the record's second member pointer). */
int daWater_Suikomi_c::State0() {
    CheckContact();
    return 1;
}

// @symbol _ZN17daWater_Suikomi_c10InitState0Ev
/* The state's entry function (the record's first member pointer). */
int daWater_Suikomi_c::InitState0() {
    return 1;
}

// @symbol _ZN17daWater_Suikomi_c8SetStateEPNS_5StateE
/* Installs a state record at +0x30c and runs its entry function. */
int daWater_Suikomi_c::SetState(State *state) {
    mState = state;
    State *s = mState;
    if (s->mInit == 0)
        return 1;
    return (this->*(s->mInit))();
}

// @symbol _ZN17daWater_Suikomi_c19func_ov026_02112324Ev
/* Empty in the ROM: four bytes, `bx lr`. Behavior still calls it. */
void daWater_Suikomi_c::func_ov026_02112324() {}

// @symbol _ZN17daWater_Suikomi_c16CleanupResourcesEv
int daWater_Suikomi_c::CleanupResources() {
    return 1;
}

// @symbol _ZN17daWater_Suikomi_c16OnPendingDestroyEv
/* fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`. */
void daWater_Suikomi_c::OnPendingDestroy() {}

// @symbol _ZN17daWater_Suikomi_c6RenderEv
/* Draws nothing: the suction has no model of its own. */
int daWater_Suikomi_c::Render() {
    return 1;
}

// @symbol _ZN17daWater_Suikomi_c8BehaviorEv
int daWater_Suikomi_c::Behavior() {
    DecIfAbove0_Short((u16 *)&mStateTimer);
    State *s = mState;
    if (s->mMain != 0) {
        (this->*(s->mMain))();
    }
    UpdatePos(&mdCcAcPos_c);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov026_02112324();
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN17daWater_Suikomi_c13InitResourcesEv
int daWater_Suikomi_c::InitResources() {
    V3 vec;
    unk_314 = param1 & 0xff;
    vec = data_ov026_02113f4c;
    mdCcAcPos_c.Init(this, *(Vector3 *)&vec, 0x64000, 0xa4000, 0x800006, 0);
    SetState(&data_ov026_02113f58.state);
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daWater_Suikomi_c through
 * RTTI, allocation size, vtable identity, and the WATER_SUIKOMI registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: WaterSuction_Spawn. */
// @symbol daWater_Suikomi_c_classInit
extern "C" daWater_Suikomi_c *daWater_Suikomi_c_classInit() {
    return new daWater_Suikomi_c();
}

/* Static-init global (was the handwritten __sinit_ov026_02112d68 shard):
 * the two state handlers, plain-copied from the ROM constants in retail
 * order. The trailing fill runs to the 0x02113f80 section end, which dsd
 * sizes the trailing symbol to; the initializer never touches it and it is
 * zeros (kuriking precedent). */
SuikomiStateTable data_ov026_02113f58 = {
    {data_ov026_02113dd8, data_ov026_02113dd0},
    {},
};
