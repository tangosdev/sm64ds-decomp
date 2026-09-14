//cpp
/* PROMOTED translation unit -- ov029/daObjWc_Obj03_c (10 function(s)).
 * deslop
 *
 * Wet-Dry World's water diamond (profile WATER_DIAMOND, actor 97): a spinning
 * crystal that, when Mario touches it, sets the WDW water (daObjWc_Mizu_c,
 * actor 0x65) target height to this diamond's Y.
 *
 * This one file is the production source for the whole class: the ROM build
 * takes every one of these functions from a single object, the way the
 * cartridge's own build did. It licenses the contiguous .text run
 * 0x02111728..0x02111ac4 in ov029 (config/tu_manifest.d/ov029/daObjWc_Obj03_c.json,
 * config/arm9/overlays/ov029/delinks.txt).
 *
 * WRITTEN IN REVERSE ROM ORDER. mwccarm emits one .text section per function
 * in the reverse of source order, so the highest-address ROM function is
 * written FIRST and the lowest last. Do not reorder these blocks. The
 * destructor is not written here at all; it is inline in
 * include/daObjWc_Obj03_c.h, declared last, and so emits first, which is
 * where the cartridge has it.
 *
 * Leftover:
 * - dCcAc_c::Init stays the scalar mangled ABI (InitResources). A real member
 *   call homes Fix12<int> by value onto the stack (notes/mwccarm-codegen.md 6az).
 * - data_ov029_02114270 is the model SharedFilePtr; symbols.txt has no recovered
 *   name, so it is not coined.
 * - Matrix4x3_FromRotationY stays an extern "C" free function
 *   (UpdateModelTransform).
 * - Address-valued `&mAngleY` in Behavior: a named increment CSEs the field
 *   address. The cartridge keeps r1 as &mAngleY for both the halfword load
 *   and store.
 * - Class header first so Model.h's structured Matrix4x3 (`.t`) wins;
 *   daObjWc_Mizu_c.h would otherwise pull common.h's flat `s32 m[12]`.
 *
 * Absorbed these legacy one-function sources (ROM address order), all deleted
 * by tools/tu_promote.py:
 *   [0] 0x02111728  src/_ZN15daObjWc_Obj03_cD1Ev.cpp
 *   [1] 0x02111760  src/_ZN15daObjWc_Obj03_cD0Ev.cpp
 *   [2] 0x021117ac  src/_ZN15daObjWc_Obj03_c19CheckClsnWithPlayerEv.cpp
 *   [3] 0x02111850  src/_ZN15daObjWc_Obj03_c10SetWaterIDEv.cpp
 *   [4] 0x021118c8  src/_ZN15daObjWc_Obj03_c20UpdateModelTransformEv.cpp
 *   [5] 0x02111908  src/_ZN15daObjWc_Obj03_c16CleanupResourcesEv.cpp
 *   [6] 0x0211192c  src/_ZN15daObjWc_Obj03_c6RenderEv.cpp
 *   [7] 0x02111954  src/_ZN15daObjWc_Obj03_c8BehaviorEv.cpp
 *   [8] 0x02111a04  src/_ZN15daObjWc_Obj03_c13InitResourcesEv.cpp
 *   [9] 0x02111a84  src/daObjWc_Obj03_c_classInit.c
 */

/* Class header first: Model.h pulls the structured Matrix4x3 (`.t`) that
 * UpdateModelTransform writes. daObjWc_Mizu_c.h -> dBgActor_c.h would otherwise
 * reach common.h's flat `s32 m[12]` spelling first. */
#include "daObjWc_Obj03_c.h"
#include "daObjWc_Mizu_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"

extern "C" {
/* dCcAc_c::Init's true signature takes Fix12<int> by value. A faithful C++
 * call homes those arguments to the stack and grows InitResources. */
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
    void *, dActor_c *, Fix12i, Fix12i, u32, u32);
void Matrix4x3_FromRotationY(Matrix4x3 *, int);
extern SharedFilePtr data_ov029_02114270;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol daObjWc_Obj03_c_classInit
/* Historical alias: WaterDiamond_Spawn -- this entry point carried the
 * project's coined class name until the ROM's RTTI name was adopted.
 * The ROM proves the class through RTTI (_ZTS15daObjWc_Obj03_c at
 * 0x02113d7c), the 0x160 allocation, the vtable at 0x02113db4 and
 * the WC_OBJ03 registry profile at 0x02113d90.
 *
 * Every instruction the cartridge has here falls out of the one `new`.
 * 0x02111a88 loads 0x160 -- the class's own size -- into the header's leaf
 * operator new; 0x02111a98 calls dActor_c's C2, the store at 0x02111aa4 lays
 * down this class's vptr, and the two member ctors (Model at 0xd4, dCcAc_c
 * at 0x124) follow. The null check is the one `new` itself emits. The
 * implicit constructor is what keeps the intervening construction inlined;
 * declaring a constructor of our own would emit a `bl` the factory does not
 * have. */
extern "C" daObjWc_Obj03_c *daObjWc_Obj03_c_classInit()
{
    return new daObjWc_Obj03_c();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c13InitResourcesEv
int daObjWc_Obj03_c::InitResources()
{
    mModel.SetFile((BMD_File *)Model::LoadFile(data_ov029_02114270), 1, -1);
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mCylinder, this, 0x32000, 0x64000, 0x800002, 0);
    mWaterID = 0;
    mWaterParam = param1 & 1;
    mActive = 0;
    mAngleY = 0;
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c8BehaviorEv
int daObjWc_Obj03_c::Behavior()
{
    SetWaterID();
    if (mWaterID == 0) return 1;
    CheckClsnWithPlayer();
    if (mActive != 0) {
        /* The address-valued temporary is load-bearing under 2004/b56: the
         * cartridge keeps r1 as &mAngleY for both the halfword load and store.
         * A named increment CSEs the field address. */
        s16 *angleY = &mAngleY;
        *angleY = *angleY + 0x400;
        daObjWc_Mizu_c *water = (daObjWc_Mizu_c *)dActor_c::FindWithID(mWaterID);
        if (water != 0) {
            if (mPosY == water->mPosY) {
                if (mAngleY == 0) mActive = 0;
            }
            if (mPosY != water->mTargetPosY) {
                mActive = 0;
                mAngleY = 0;
            }
        }
    }
    UpdateModelTransform();
    mCylinder.Clear();
    mCylinder.Update();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c6RenderEv
int daObjWc_Obj03_c::Render()
{
    mModel.Render(0);
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c16CleanupResourcesEv
/* daObjWc_Obj03_c::CleanupResources -- vtable slot 3. Releases the one shared
 * file the class holds; it never touches `this`, which is why the legacy C form
 * could declare itself nullary and still reproduce. */
int daObjWc_Obj03_c::CleanupResources()
{
    data_ov029_02114270.Release();
    return 1;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c20UpdateModelTransformEv
void daObjWc_Obj03_c::UpdateModelTransform()
{
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.t.x = mPosX >> 3;
    mModel.mat4x3.t.y = mPosY >> 3;
    mModel.mat4x3.t.z = mPosZ >> 3;
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c10SetWaterIDEv
void daObjWc_Obj03_c::SetWaterID()
{
  dActor_c *water;
  if (mWaterID != 0) return;
  water = 0;
  for (;;) {
    water = dActor_c::FindWithActorID(0x65, water);
    if (water == 0) goto done;
    if (mWaterParam == ((daObjWc_Mizu_c *)water)->mUseSpawnPosY) break;
  }
  mWaterID = water->uniqueID;
done:
  if (mWaterID != 0) return;
  MarkForDestruction();
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_c19CheckClsnWithPlayerEv
void daObjWc_Obj03_c::CheckClsnWithPlayer()
{
    if (mActive != 0)
        return;
    u32 id = mCylinder.otherOwner;
    if (id == 0)
        return;
    dActor_c *actor = dActor_c::FindWithID(id);
    if (actor == 0)
        return;
    int isPlayer = (int)(actor->actorID == 0xbf);
    if (isPlayer == 0)
        return;
    daObjWc_Mizu_c *water = (daObjWc_Mizu_c *)dActor_c::FindWithID(mWaterID);
    int targetPosY = water->mTargetPosY;
    if (targetPosY != water->mPosY)
        return;
    if (targetPosY == mPosY)
        return;
    mActive = 1;
    water->mTargetPosY = mPosY;
    Sound::PlayBank3(0x63, *(Vector3 *)&mCamSpacePosX);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
// @symbol _ZN15daObjWc_Obj03_cD1Ev
// @symbol _ZN15daObjWc_Obj03_cD0Ev
/* NOT WRITTEN HERE ON PURPOSE. The destructor body is INLINE in
 * include/daObjWc_Obj03_c.h and declared LAST. Two measurements force that:
 *   - out of line, mwcc emits D0 ahead of D1 and the cartridge has D1 first,
 *     which rombuild refuses outright;
 *   - out of line it also emits the D2 base-object variant, which the ROM
 *     never carried.
 * Declaring it last, with the factory's `new` instantiating the class, is
 * what emits the used D1/D0 pair in cartridge order and keeps this TU the
 * vtable's home.
 *
 * Both bodies destroy dCcAc_c at +0x124 and Model at +0xd4 in reverse
 * construction order, then chain to dActor_c. D0 then returns the object to
 * the actor heap through dActor_c's inline operator delete.
 */
