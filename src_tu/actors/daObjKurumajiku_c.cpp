//cpp
/**
 * The rickshaw axle (`kurumajiku`): an abstract dBgActor_c that spins and
 * drags up to four mounted carts around itself.
 *
 * Shared base of two overlay actors that are already promoted --
 * ov043 daObjKm1_Kurumajiku_c (src/game/actors/d_a_obj_km1_kurumajiku.cpp)
 * and ov047 daObjKm3_Kurumajiku_c (src/game/actors/d_a_obj_km3_kurumajiku.cpp).
 * Both leave InitResources / CleanupResources pure here and hand this
 * overlay's two helpers their own resource descriptor, which is why those
 * two helpers live in ov002 with the base rather than in either child.
 *
 * The class is ABSTRACT: vtable slots 0 and 3 are null words in the
 * cartridge. include/daObjKurumajiku_c.h spells that with pure-specifiers
 * and explains why. tools/romdata_check.py confirms the model against the
 * cartridge: the vtable this TU emits is byte-equal to ov002 0x02109320
 * for all 128 bytes, and the class RTTI record and type-name string are
 * byte-equal at 0x021092f8 and 0x02109304.
 *
 * deslop
 * Leftover: func_ov002_020b6a80 and func_ov002_020b6ac8 are still under
 *   their linker names. 020b6ac8 is the shared CleanupResources helper
 *   (disable the collision, release the two SharedFilePtrs); 020b6a80
 *   rebuilds the collision matrix from the actor's own angles and scales
 *   its position by 1/8. Naming them is a separate rename PR.
 * Leftover: this TU is text-only. The vtable/RTTI group it emits is
 *   verified at its cartridge home and then discarded by isolation; the
 *   cartridge's own copy is supplied by the overlay's data gap.
 * Leftover: NOT PROMOTED. The destructor pair cannot be emitted in the
 *   cartridge's order from this slice -- see the manifest's
 *   `destructor_order_blocker` note for the measurements.
 */

#include "daObjKurumajiku_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"

extern "C" {
extern Vector3 data_ov002_0210ddd0[4];
extern char data_020a0e68[0x30];

void Matrix4x3_FromRotationZXYExt(void *dst, int x, int y, int z);
void Matrix4x3_FromRotationXYZExt(void *m, int x, int y, int z);
void MulVec3Mat4x3(const Vector3 *v, const void *m, Vector3 *dst);
void Vec3_Add(Vector3 *out, const Vector3 *a, const Vector3 *b);

int func_ov002_020b6a80(daObjKurumajiku_c *self);
int func_ov002_020b6ac8(daObjKurumajiku_c *self, void *descriptor);

/* Carries Fix12<int> by value, so its definition stays a mangled free
   function -- see include/ShadowModel.h. A call is unaffected. */
int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(dBgActor_c *self, int x, int z);
}

/* mwccarm 2004/b56 emits one .text section per function in the REVERSE of
   source order, so the highest-address cartridge function is written first. */

// @symbol _ZN17daObjKurumajiku_c8BehaviorEv
/* Vtable slot 6, ov002 0x020b6b38. The axle turns 0x100 per frame and drags
   its riders with it: each rider's offset comes out of the four-entry table
   at data_ov002_0210ddd0, is rotated by the axle's own Euler angles through
   the scratch matrix at data_020a0e68, and is added to the axle's position.
   Bit 3 of mFlags -- fBase_c's "behaviour off" bit -- suppresses all of it,
   and the early return skips the collision update too, which is what makes
   a stopped axle stop carrying. */
s32 daObjKurumajiku_c::Behavior()
{
    int i;
    Vector3 mid;
    Vector3 res;
    s16 *rzp = &mAngleZ;

    *rzp = *rzp + 0x100;
    {
        int suppressed = (int)((mFlags & 8) != 0);
        if (suppressed != 0)
            return 1;
    }
    for (i = 0; i < 4; i++) {
        if (mMountedActorIds[i]) {
            dActor_c *rider = dActor_c::FindWithID(mMountedActorIds[i]);
            if (rider) {
                mid.x = 0;
                mid.y = 0;
                mid.z = 0;
                Matrix4x3_FromRotationXYZExt(data_020a0e68, mAngleX, mAngleY, mAngleZ);
                MulVec3Mat4x3(&data_ov002_0210ddd0[i], data_020a0e68, &mid);
                Vec3_Add(&res, (Vector3 *)&mPosX, &mid);
                rider->mPosX = res.x;
                rider->mPosY = res.y;
                rider->mPosZ = res.z;
            }
        }
    }
    func_ov002_020b6a80(this);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(this, 0, 0))
        UpdateClsnPosAndRot();
    return 1;
}

// @symbol _ZN17daObjKurumajiku_c6RenderEv
/* Vtable slot 9, ov002 0x020b6b10. Draws through dBgActor_c's own mModel. */
s32 daObjKurumajiku_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol func_ov002_020b6ac8
/* ov002 0x020b6ac8 -- the CleanupResources body both descendants call with
   their own resource descriptor: drop the collision if it is enabled, then
   release the descriptor's model and collision SharedFilePtrs. The
   descriptor stays void * here because only the two callers know its shape;
   ov043 and ov047 each declare their own typed view of it. */
extern "C" int func_ov002_020b6ac8(daObjKurumajiku_c *self, void *descriptor)
{
    char *r4 = (char *)descriptor;
    if (((dBgW *)((char *)self + 0x124))->IsEnabled())
        ((dBgW *)((char *)self + 0x124))->Disable();
    ((SharedFilePtr *)(*(void **)r4))->Release();
    ((SharedFilePtr *)(*(void **)(r4 + 4)))->Release();
    return 1;
}

// @symbol func_ov002_020b6a80
/* ov002 0x020b6a80 -- rebuild the collision matrix from the actor's own
   Y/Z angles and copy its position in at 1/8 scale. Reached by offset
   because this one is still under its linker name; the offsets are
   dBgActor_c's mClsnMtx (0xf0) and its position row (0x114). */
extern "C" int func_ov002_020b6a80(daObjKurumajiku_c *self)
{
    char *c = (char *)self;
    Matrix4x3_FromRotationZXYExt(c + 0xf0, 0, *(short *)(c + 0x8e), *(short *)(c + 0x90));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = *(int *)(c + 0x60) >> 3;
    int z = *(int *)(c + 0x64) >> 3;
    *(int *)(c + 0x11c) = z;
    return z;
}
