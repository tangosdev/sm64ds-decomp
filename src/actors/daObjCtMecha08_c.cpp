//cpp
/**
 * Tick Tock Clock's up-and-down lift block (registry profiles CT_MECHA08A and
 * CT_MECHA08B).
 *
 * One class behind two profiles. InitResources loads the model and the
 * collision mesh, remembers where it was placed as its home, and seeds its
 * vertical speed and travel timer from two per-clock-setting tables.
 *
 * Behavior runs the lift between mHomePosY and mHomePosY + 0x14a000. Each time
 * mMoveTimer runs out it clamps itself back inside that span, flips mMoveDir
 * and reloads speed and timer for the new direction. Clock setting 2 -- the
 * random hand -- re-rolls the timer instead; setting 3 stops the clock, which
 * parks the lift at the top of its travel.
 *
 * This TU owns text only: ov065 delinks no .data here, so the _ZTV / _ZTI /
 * _ZTS group the class names is compiler-only output, compared against the
 * cartridge's own copies at ov065 0x0211d494 / 0x0211d434 / 0x0211d440.
 *
 * SOURCE ORDER IS REVERSE ROM ORDER. mwccarm 2004/b56 emits .text back to
 * front under this tree's flags, so InitResources is written first and
 * CleanupResources last; the destructor pair comes off the in-class
 * `~daObjCtMecha08_c() {}` in include/daObjCtMecha08_c.h and lands ahead of
 * everything, D1 then D0, which is the order the cartridge has (0x0211b7f0,
 * then 0x0211b834).
 *
 * deslop
 * Leftover: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange stay spelled as
 *   mangled extern-C free functions. Each takes Fix12<int> by value, and a
 *   real method call homes the argument and size-DIFFs the caller
 *   (notes/mwccarm-codegen.md 6az); include/dBgW_KcMbg.h records the same.
 * Leftover: DecIfAbove0_Short and RandomIntInternal keep linker names -- no
 *   header home, the daObjCtMecha10_c precedent.
 * Leftover: Behavior's clamp keeps the ROM's goto shape, and its mMoveDir
 *   toggle goes through a laundered address; both are measured below.
 * Leftover: TtcRotatingGear_ModelFile / TtcRotatingGear_ClsnFile are this
 *   class's two shared-file handles in ov065 .bss (0x0211d98c / 0x0211d97c),
 *   still under the coined spelling symbols.txt gives them.
 *   data_ov065_0211c0d0 / 0211c0d4 are the per-setting speed and timer
 *   tables. None of them is owned by this TU.
 * Leftover: data_ov035_021121b8 is the CLPS block, in the Tick Tock Clock
 *   level overlay. ov065's own relocation lists ten overlays that hold
 *   0x021121b8; tools/overlay_residency.py settles them to ov035, the level
 *   overlay this object overlay is loaded beside.
 * Leftover: data_0209f2c0 is arm9's clock-setting byte and data_0209e650 the
 *   shared RNG state.
 * Leftover: g_profile_CT_MECHA08A / CT_MECHA08B and their two classInit
 *   factories live outside this TU, at 0x0211bb7c and above.
 */

#include "daObjCtMecha08_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

struct CLPS_Block;

extern "C" {
extern SharedFilePtr TtcRotatingGear_ModelFile;
extern SharedFilePtr TtcRotatingGear_ClsnFile;
extern CLPS_Block    data_ov035_021121b8;
extern s32 data_ov065_0211c0d0[][2][2];     /* vertical speed, by setting, by direction */
extern u16 data_ov065_0211c0d4[][2][4];     /* travel timer, by setting, by direction */
extern u8  data_0209f2c0;                   /* arm9 clock setting */
extern int data_0209e650;                   /* arm9 RNG state */

u16 DecIfAbove0_Short(u16 *p);
int RandomIntInternal(int *state);

int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
void _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
    dBgW_KcMbg *self, KCL_File *file, const Matrix4x3 *mat,
    Fix12i scale, s16 angle, CLPS_Block *clps);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha08_c13InitResourcesEv
/* Load both files, remember the placed position as home, and start moving in
   direction 0 with that direction's speed and timer for the current clock
   setting. */
int daObjCtMecha08_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(TtcRotatingGear_ModelFile), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    _ZN10dBgW_KcMbg7SetFileEP8KCL_FileRK9Matrix4x35Fix12IiEsR10CLPS_Block(
        &mMeshCollider,
        (KCL_File *)dBgW_Kc::LoadFile(TtcRotatingGear_ClsnFile),
        &mClsnMat, 0x199, mAngleY, &data_ov035_021121b8);

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mMoveDir = 0;

    u8 setting = data_0209f2c0;
    mMoveTimer = data_ov065_0211c0d4[setting][mMoveDir][0];
    mVertSpeed = data_ov065_0211c0d0[setting][mMoveDir][0];
    mTerminalVelocity = -0x3c000;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha08_c8BehaviorEv
/* Setting 3 parks the lift at the top of its travel. Otherwise it moves under
   its own speed, and each time the timer runs out it clamps back inside
   [mHomePosY, mHomePosY + 0x14a000], turns round, and reloads speed and timer.
   Setting 2 overrides the timer with a random multiple of 0x14 in [0xa, 0x6e].

   MEASURED: the control flow is the ROM's, gotos and all -- the in-range test
   reads as an if/else but compiles to the cartridge's interleaved form only in
   this shape. */
int daObjCtMecha08_c::Behavior()
{
    if (data_0209f2c0 == 3) {
        mPosY = mHomePosY + 0x14a000;
        UpdateModelPosAndRotY();
        if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
            UpdateClsnPosAndRot();
        goto done;
    }

    if (DecIfAbove0_Short(&mMoveTimer) == 0) {
        s32 lower, upper, y, inRange;

        UpdatePos(0);
        lower = mHomePosY;
        y = mPosY;
        upper = lower + 0x14a000;
        if (y < lower)
            goto outside;
        if (y <= upper) {
            inRange = 1;
            goto test;
        }
outside:
        inRange = 0;
test:
        if (inRange != 0)
            goto update;
        {
            s32 v = lower;
            if (y >= lower) {
                s32 t = upper;
                if (y <= upper)
                    t = y;
                v = t;
            }
            mPosY = v;
        }

        {
            /* MEASURED: THE LAUNDERED ADDRESS IS LOAD-BEARING. Spelling this
               as `mMoveDir ^= 1` or as a load/store pair through the member
               changes the function's size, not just its allocation -- the
               RMW-address CSE described in notes/mwccarm-codegen.md. Taking
               the address through an integer first defeats it. */
            u8 *dirp = (u8 *)((int)this + 0x32e);
            u8 dir = *dirp;
            *dirp = dir ^ 1;
            {
                int setting = data_0209f2c0;
                u8 d = mMoveDir;
                mMoveTimer = data_ov065_0211c0d4[setting][d][0];
                d = mMoveDir;
                mVertSpeed = data_ov065_0211c0d0[setting][d][0];
                if (data_0209f2c0 != 2)
                    goto update;
                {
                    u32 rnd = (u32)RandomIntInternal(&data_0209e650);
                    mMoveTimer = (u16)((rnd % 6) * 0x14 + 0xa);
                }
            }
        }
    }

update:
    UpdateModelPosAndRotY();
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0) != 0)
        UpdateClsnPosAndRot();
done:
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha08_c6RenderEv
int daObjCtMecha08_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN16daObjCtMecha08_c16CleanupResourcesEv
/* Give back exactly what InitResources took: the collider, then both files. */
int daObjCtMecha08_c::CleanupResources()
{
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    TtcRotatingGear_ModelFile.Release();
    TtcRotatingGear_ClsnFile.Release();
    return 1;
}
