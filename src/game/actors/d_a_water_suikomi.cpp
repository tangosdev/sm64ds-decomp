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
 * This is the whole unit: the destructor pair, the five state-machine
 * helpers between D0 and CleanupResources, the five virtuals, then the
 * factory. The helpers are not virtuals: 0x021122cc and 0x021122b0 are the two
 * member-function pointers that ov026's last static initializer (0x02112d68)
 * copies from 0x02113dd8/0x02113dd0 into the state record at 0x02113f58;
 * func_ov026_021122d4 installs a state record at +0x30c (InitResources hands
 * it 0x02113f58); and Behavior calls func_ov026_02112324. (A vtable scan
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
 * leftovers:
 * - The helpers keep their func_ov026_* names and C linkage: the static
 *   initializer and the state record name them by address, and none has a
 *   recovered name.
 * - dCcAcPos_c::Init stays the mangled extern (InitResources): the real
 *   member takes its radius and height as Fix12<int> by value and mwccarm
 *   homes both in stack temporaries; measured 0x88 -> 0x98 with a 0x20
 *   frame instead of 0x18.
 */
#include "decl_common.h"
#include "daWater_Suikomi_c.h"

/* The state record at +0x30c: an enter call at +0, then the per-frame call
 * at +8 that Behavior makes. SuikomiState stands in for the class the
 * member-function pointers are typed against. */
struct SuikomiStateRec;
struct SuikomiState {
    char pad[0x30c];
    SuikomiStateRec *state;
};
typedef void (SuikomiState::*SuikomiStateFn)();
struct SuikomiStateRec {
    char pad[8];
    SuikomiStateFn execute;
};

/* The ROM member-function-pointer constants the state table copies from
 * (unlicensed .data). */
extern SuikomiStateFn data_ov026_02113dd8;
extern SuikomiStateFn data_ov026_02113dd0;

/* The state table at 0x02113f58: the two handlers this TU's InitResources
 * hands to func_ov026_021122d4, plus section-end fill (see the definition
 * at the end of this file). */
struct SuikomiStateTable {
    SuikomiStateFn handlers[2];
    u8 tailFill[0x18];
};
extern SuikomiStateTable data_ov026_02113f58;

extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
struct V3 {
    int x, y, z;
};
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *thiz, void *actor, V3 *vec, int a, int b, unsigned int cc, unsigned int d);
extern V3 data_ov026_02113f4c;
}

#pragma defer_codegen off

// @symbol _ZN17daWater_Suikomi_cD1Ev
// @symbol _ZN17daWater_Suikomi_cD0Ev
/* The key function. D1 stores the vtable, destroys mWithMeshClsn (+0x150)
 * and mdCcAcPos_c (+0x110) in reverse declaration order, then runs
 * dEnemyBase_c's D2; D0 does the same and hands the object to the inline
 * operator delete. The base-object D2 mwcc also emits has no home in the
 * cartridge. */
daWater_Suikomi_c::~daWater_Suikomi_c()
{
}

// @symbol func_ov026_02112280
/* Looks up the actor the collider last touched (mdCcAcPos_c.otherOwner) and
 * does nothing with it: the ROM body returns either way. */
extern "C" void func_ov026_02112280(char *c)
{
    unsigned int id = ((daWater_Suikomi_c *)c)->mdCcAcPos_c.otherOwner;
    if (id == 0) return;
    void *r = dActor_c::FindWithID(id);
    if (r == 0) return;
}

// @symbol func_ov026_021122b0
/* The state's per-frame function (the record's second member pointer). */
extern "C" int func_ov026_021122b0(void *t)
{
    func_ov026_02112280((char *)t);
    return 1;
}

// @symbol func_ov026_021122cc
/* The state's entry function (the record's first member pointer). */
extern "C" int func_ov026_021122cc(void)
{
    return 1;
}

/* func_ov026_021122d4's view of the object: the state record pointer at
 * +0x30c, as a pointer to the record's first (entry) member pointer. */
struct SuikomiStateOwner;
typedef int (SuikomiStateOwner::*SuikomiEnterFn)();
struct SuikomiStateOwner {
    char pad[0x30c];
    SuikomiEnterFn *pp;
};

// @symbol func_ov026_021122d4
/* Installs a state record at +0x30c and runs its entry function. */
extern "C" int func_ov026_021122d4(void *thiz, void *rec)
{
    SuikomiStateOwner *c = (SuikomiStateOwner *)thiz;
    c->pp = (SuikomiEnterFn *)rec;
    SuikomiEnterFn *q = c->pp;
    if (*q == 0) return 1;
    return (c->**q)();
}

// @symbol func_ov026_02112324
/* Empty in the ROM: four bytes, `bx lr`. Behavior still calls it. */
extern "C" void func_ov026_02112324(void *)
{
}

// @symbol _ZN17daWater_Suikomi_c16CleanupResourcesEv
int daWater_Suikomi_c::CleanupResources()
{
    return 1;
}

// @symbol _ZN17daWater_Suikomi_c16OnPendingDestroyEv
/* fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`. */
void daWater_Suikomi_c::OnPendingDestroy()
{
}

// @symbol _ZN17daWater_Suikomi_c6RenderEv
/* Draws nothing: the suction has no model of its own. */
int daWater_Suikomi_c::Render()
{
    return 1;
}

// @symbol _ZN17daWater_Suikomi_c8BehaviorEv
int daWater_Suikomi_c::Behavior()
{
    SuikomiState *s = (SuikomiState *)this;
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    SuikomiStateRec *rec = s->state;
    if (rec->execute != 0) {
        (s->*(rec->execute))();
    }
    UpdatePos(&mdCcAcPos_c);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov026_02112324(this);
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN17daWater_Suikomi_c13InitResourcesEv
int daWater_Suikomi_c::InitResources()
{
    V3 vec;
    unk_314 = param1 & 0xff;
    vec = data_ov026_02113f4c;
    /* See leftovers: the mangled extern stands in for the Fix12-by-value
       member. */
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &vec, 0x64000, 0xa4000, 0x800006, 0);
    func_ov026_021122d4(this, &data_ov026_02113f58);
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daWater_Suikomi_c through
 * RTTI, allocation size, vtable identity, and the WATER_SUIKOMI registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: WaterSuction_Spawn. */
// @symbol daWater_Suikomi_c_classInit
extern "C" daWater_Suikomi_c *daWater_Suikomi_c_classInit()
{
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
