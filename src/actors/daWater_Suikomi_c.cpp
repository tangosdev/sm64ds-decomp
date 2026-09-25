//cpp
/* Production translation unit for ov026/daWater_Suikomi_c.
 * 5 function(s), .text 0x02112328..0x02112450. The WATER_SUIKOMI actor, the
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
 * This is a partial span of the class: five complete delinks that abut, with
 * nothing between them. D1 and D0 keep their own files, because five
 * unconverted one-function sources sit between D0 and CleanupResources
 * (func_ov026_02112280..func_ov026_02112324). They are this class's
 * state-machine helpers, not virtuals: 0x021122cc and 0x021122b0 are the two
 * member-function pointers that ov026's last static initializer (0x02112d68)
 * copies from 0x02113dd8/0x02113dd0 into the state record at 0x02113f58;
 * func_ov026_021122d4 installs a state record at +0x30c (InitResources hands
 * it 0x02113f58); and Behavior calls func_ov026_02112324. The out-of-line
 * destructor is the key function and is defined in
 * src/_ZN17daWater_Suikomi_cD1Ev.cpp and src/_ZN17daWater_Suikomi_cD0Ev.cpp,
 * so _ZTV/_ZTI/_ZTS are emitted there and this TU emits none of them. The
 * factory daWater_Suikomi_c_classInit starts at 0x02112450
 * (src/d_a_water_suikomi.c) and is not absorbed.
 *
 * Under `#pragma defer_codegen off` .text is laid down in source order, so
 * this file is ROM-ascending.
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

extern "C" {
extern unsigned short DecIfAbove0_Short(unsigned short *p);
struct V3 {
    int x, y, z;
};
extern void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *thiz, void *actor, V3 *vec, int a, int b, unsigned int cc, unsigned int d);
extern V3 data_ov026_02113f4c;
}

#pragma defer_codegen off

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
    /* Leftover: dCcAcPos_c::Init stays the mangled extern. The real member
       takes its radius and height as Fix12<int> by value, and mwccarm homes
       both in stack temporaries before the call: measured, InitResources
       grows from 0x88 to 0x98 bytes with a 0x20 frame instead of 0x18. */
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(&mdCcAcPos_c, this, &vec, 0x64000, 0xa4000, 0x800006, 0);
    func_ov026_021122d4(this, &data_ov026_02113f58);
    return 1;
}
