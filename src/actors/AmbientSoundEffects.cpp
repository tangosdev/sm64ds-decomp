//cpp
/* Manually curated translation unit, reconstructed and promoted.
 * ov002/AmbientSoundEffects  (8 function(s))
 *
 * CANONICAL. The readable class and member definitions below are what the ROM
 * build compiles; the eight legacy production sources they replaced are gone.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function, in the REVERSE of source order, so
 * the highest-address ROM function is written FIRST here. Do not reorder;
 * see notes/tu-reconstruction-pilot-report.md sec 3 for the one documented
 * exception (a destructor's D0/D1/D2 group has compiler-chosen order).
 *
 * Assembled from these legacy one-function sources (ROM address order):
 *   [0] 0x020f198c  src/_ZN19AmbientSoundEffectsD1Ev.cpp
 *   [1] 0x020f19b0  src/_ZN19AmbientSoundEffectsD0Ev.cpp
 *   [2] 0x020f19e8  src/_ZN19AmbientSoundEffects16CleanupResourcesEv.cpp
 *   [3] 0x020f19f0  src/_ZN19AmbientSoundEffects16OnPendingDestroyEv.cpp
 *   [4] 0x020f19f4  src/_ZN19AmbientSoundEffects6RenderEv.cpp
 *   [5] 0x020f19fc  src/_ZN19AmbientSoundEffects8BehaviorEv.cpp
 *   [6] 0x020f1ac4  src/_ZN19AmbientSoundEffects13InitResourcesEv.cpp
 *   [7] 0x020f1b94  src/AmbientSoundEffects_Spawn.c
 *
 * Naming boundary: these configured text symbols use AmbientSoundEffects,
 * while the adjacent retail RTTI spells daSetSE_c. Until that symbol migration
 * is resolved, this TU claims only the eight verified text contributions; its
 * generated RTTI/vtable is evidence, not licensed retail data.
 */

#include "AmbientSoundEffects.h"
#include "Camera.h"

extern "C" {
extern int _ZTV19AmbientSoundEffects[];
extern void *_ZN7fBase_cnwEj(unsigned);
extern void _ZN8dActor_cC2Ev(void *);
extern u32 _ZN5Sound8PlayLongEjjjRK7Vector3s(u32 a, u32 b, u32 c, void *v, u32 e);
extern int data_0209b4ac;
extern int data_ov002_0210b498[];
extern int data_ov002_02110aec;
extern Camera *data_0209f318;
extern int IsStarCollectedInLevel(s8 levelID, int starID);
extern s8 data_0209f2f8;
extern u8 data_0209f220;
extern u8 data_0209f2d8;
extern int data_0209caa0[];
extern int data_0209fc48;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 7 -- AmbientSoundEffects_Spawn, 0x020f1b94, size 0x30 */
/* -------------------------------------------------------------------------- */
// @symbol AmbientSoundEffects_Spawn
extern "C" AmbientSoundEffects *AmbientSoundEffects_Spawn(void)
{
    AmbientSoundEffects *p =
        (AmbientSoundEffects *)_ZN7fBase_cnwEj(sizeof(AmbientSoundEffects));
    if (p) {
        _ZN8dActor_cC2Ev(p);
        *(int *)p = (int)&_ZTV19AmbientSoundEffects[2];
    }
    return p;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN19AmbientSoundEffects13InitResourcesEv, 0x020f1ac4, size 0xd0 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19AmbientSoundEffects13InitResourcesEv
int AmbientSoundEffects::InitResources()
{
    int flag;

    if (data_0209f2f8 == 8 && (data_0209f220 == 1 || IsStarCollectedInLevel(8, 1) == 0))
        return 0;

    /* Volatile keeps CW from materializing an extra base pointer for this
       inherited-field RMW; the ROM loads and stores [this + 0x08] directly. */
    u32 soundID = *(volatile u32 *)((char *)this + 0x8);
    *(volatile u32 *)((char *)this + 0x8) = soundID & 0xf;

    if ((int)(data_0209f2d8 == 0) != 0
        && (data_0209caa0[2] & 0x80) == 0
        && (int)(data_0209fc48 != 0) == 0)
        flag = 1;
    else
        flag = 0;

    data_ov002_02110aec = flag;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN19AmbientSoundEffects8BehaviorEv, 0x020f19fc, size 0xc8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19AmbientSoundEffects8BehaviorEv
int AmbientSoundEffects::Behavior()
{
    u32 param;
    int a;
    Camera *cam;

    if (data_ov002_02110aec != 0)
        return 1;

    param = param1;
    if (param >= 1 && param <= 4) {
        cam = data_0209f318;
        a = data_0209b4ac;
        if (a == 0x32 || a == 0x33 || a == 0x34 ||
            cam->IsUnderwater())
            return 1;
    }

    mSoundHandle = _ZN5Sound8PlayLongEjjjRK7Vector3s(
        mSoundHandle, 3, (u32)data_ov002_0210b498[param1],
        &mCamSpacePosX, 0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN19AmbientSoundEffects6RenderEv, 0x020f19f4, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19AmbientSoundEffects6RenderEv
int AmbientSoundEffects::Render()
{
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN19AmbientSoundEffects16OnPendingDestroyEv, 0x020f19f0, size 0x4 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19AmbientSoundEffects16OnPendingDestroyEv
void AmbientSoundEffects::OnPendingDestroy()
{
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN19AmbientSoundEffects16CleanupResourcesEv, 0x020f19e8, size 0x8 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN19AmbientSoundEffects16CleanupResourcesEv
int AmbientSoundEffects::CleanupResources()
{
    return 1;
}

/* ROM ordinals 0/1 -- the compiler emits D1 and D0 from this definition. */
