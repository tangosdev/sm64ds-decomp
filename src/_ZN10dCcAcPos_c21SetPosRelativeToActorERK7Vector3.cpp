//cpp
// @symbol _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3
/* recovered: real C++ method, named members
 *
 * Places this collider at an offset expressed in the owner's local frame:
 * rotate the offset by the owner's yaw, then translate by the owner's
 * position. dCcAcPos_c::Init calls it once at construction.
 *
 * mAngleY is s16 and the ROM's load is `ldrsh' -- reading it through the named
 * member keeps the sign.
 */
#include "dCcAcPos_c.h"
#include "dActor_c.h"

extern "C" void Vec3_RotateYAndTranslate(Vector3 *res, const Vector3 *translation, s16 angY, const Vector3 *v);

void dCcAcPos_c::SetPosRelativeToActor(const Vector3 &offset)
{
    dActor_c *actor = owner;
    Vec3_RotateYAndTranslate(&pos, (const Vector3 *)&actor->mPosX, actor->mAngleY, &offset);
}
