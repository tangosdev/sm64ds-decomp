//cpp
/* Production translation unit for ov081/daSnowball_c.
 * 5 function(s), .text 0x02126298..0x021264b4. The SNOWBALL actor.
 *
 * NAME: _ZTS12daSnowball_c is "12daSnowball_c" at ov081 0x02128a88; _ZTI at
 * 0x02128a7c reads [__si_class_type_info, that string, _ZTI12dEnemyBase_c],
 * and the word before the _ZTV12daSnowball_c address point (0x02128abc)
 * points at that _ZTI. The tree previously called the class Snowball
 * (coined; that spelling is not in the cartridge).
 *
 * This is a partial span of the class: five complete delinks that abut, with
 * nothing between them. D1 (0x02125f14) and D0 (0x02125f5c) sit on the far
 * side of other functions and keep their own files. The out-of-line
 * destructor is the key function and is defined in src/_ZN12daSnowball_cD1Ev.cpp
 * and src/_ZN12daSnowball_cD0Ev.cpp, so _ZTV/_ZTI/_ZTS are emitted there and
 * this TU emits none of them. The factory daSnowball_c_classInit starts at
 * 0x021264b4 (src/d_a_snowball.c) and is not absorbed.
 *
 * decl_common.h is first so common.h's Matrix4x3 wins over math/Matrix.h.
 * Under `#pragma defer_codegen off` .text is laid down in source order, so
 * this file is ROM-ascending.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daSnowball_c.h"
#include "Player.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov081_02128d90;

extern "C" {
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *thiz, void *actor, int radius, int height, unsigned int flags, unsigned int vuln);
void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *thiz, void *actor, int radius, int height, void *a, int b);
}

/* Behavior's state call. The pointer lives in unk_378; the member
 * function sits 8 bytes into that record. */
struct SnowballState;
typedef void (SnowballState::*SnowballStateFn)();
struct SnowballStateRec { char pad[8]; SnowballStateFn fn; };

// @symbol _ZN12daSnowball_c16CleanupResourcesEv
/* Releases the one shared file InitResources claimed. Touches no field: the
 * ROM body never reads `this`, and as a method it receives one and ignores
 * it, which measured byte-free. */
int daSnowball_c::CleanupResources()
{
    data_ov081_02128d90.Release();
    return 1;
}

// @symbol _ZN12daSnowball_c16OnPendingDestroyEv
/* fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`. */
void daSnowball_c::OnPendingDestroy()
{
}

// @symbol _ZN12daSnowball_c6RenderEv
int daSnowball_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN12daSnowball_c8BehaviorEv
int daSnowball_c::Behavior()
{
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    SnowballStateRec *rec = *(SnowballStateRec **)&unk_378;
    if (rec->fn != 0)
        (((SnowballState *)(char *)this)->*(rec->fn))();
    int v = mVertSpeed + mVertAccel;
    int hi = mTerminalVelocity;
    if (v >= hi)
        hi = v;
    int tmp = unk_0ac;
    mVertSpeed = hi;
    unk_0ac = tmp;
    UpdatePosWithOnlySpeed((dCc_c *)&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);
    mAngleY = mPrevAngleY;
    func_ov081_02126224((char *)this);
    mdCcAc_c.Clear();
    Player *player = ClosestPlayer();
    if (player != 0 && player->mIsVanish == 0)
        mdCcAc_c.Update();
    return 1;
}

// @symbol _ZN12daSnowball_c13InitResourcesEv
int daSnowball_c::InitResources()
{
    void *file = Model::LoadFile(data_ov081_02128d90);
    if (mModel.SetFile((BMD_File *)file, 1, -1) == 0)
        return 0;

    mShadowModel.InitCylinder();

    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    /* Leftover: both Init calls below stay on their mangled names.
       dCcAc_c::Init takes Fix12<int> by value, and passing the radii that way
       spills them into .rodata, so the method call no longer matches.
       dBgCh_Actr::Init is declared with Fix12i, a plain s32 that mangles as
       `i`, so the method call would name a symbol other than the ROM's
       5Fix12IiE one. */
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, this, 0x1e000, 0x1e000, 0x200004, 0x40010);

    unk_37c = mPosX;
    unk_380 = mPosY;
    unk_384 = mPosZ;
    mPosY += 0x32000;
    mAngleY = mPrevAngleY;
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x14000, 0x14000, 0, 0);

    func_ov081_021261d4(this, &data_ov081_02128eb4);
    return 1;
}
