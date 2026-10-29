//cpp
/* leftovers:
 * dCcAcPos_c::GetOwnerID stays the 12-byte interworking veneer. The
 * qualified call is what -interworking lowers to `ldr ip,[pc]; bx ip`,
 * and _ZTV10dCcAcPos_c slot 3 binds to that veneer rather than to the
 * base body (dCcAcPos_c.h).
 * The two Init definitions are methods. Their radius and height are the
 * scalar Fix12i the ROM symbols encode (notes/mwccarm-codegen.md 6az).
 */
/* dCcAc_c -- collision cylinder attached to an dActor_c. Cartridge class:
   _ZTV7dCcAc_c at 0x0208e6d4, _ZTS7dCcAc_c at 0x0208e668, _ZTI7dCcAc_c at
   0x0208e698 (si_class, base dCc_c). The derived position-bearing class
   dCcAcPos_c (_ZTV10dCcAcPos_c at 0x0208e704, si_class on this class) is
   this same TU, lifecycle included. ~dCcAcPos_c() is the key function, so
   the vtable emits here; slot 3 is the veneer below, not
   dCcAc_c::GetOwnerID(). D2 and C2 have no cartridge home. TU claims
   0x0201489c..0x02014aa8. Written back-to-front: mwccarm emits .text in
   reverse source order under default deferred codegen. */
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "dActor_c.h"

// @symbol _ZN10dCcAcPos_cC1Ev
/* pos is left uninitialised. C1 is the dCcAc_c base step and this vtable
   store; C2 is the same body with no cartridge home. */
dCcAcPos_c::dCcAcPos_c() {}

// @symbol _ZN10dCcAcPos_cD1Ev
/* No member has a destructor the cartridge calls. D1 stores this vtable
   and runs dCcAc_c's D2; D0 adds the inherited operator delete. D2 is
   emitted beside them and has no cartridge home. */
dCcAcPos_c::~dCcAcPos_c() {}

// @symbol _ZN10dCcAcPos_c6GetPosEv
/* Vtable slot 2. This shadows dCcAc_c's own slot 2, which returns the
   OWNER's position; this class carries its own. */
Vector3 &dCcAcPos_c::GetPos()
{
    return pos;
}

// @symbol _ZN10dCcAcPos_c10GetOwnerIDEv
/* Vtable slot 3. The body is the qualified base call, not a copy of the
   base body: -interworking lowers it to the 12-byte veneer
   `ldr ip,[pc]; bx ip; .word <dCcAc_c::GetOwnerID()>`. Declaring the
   override is what makes the emitted vtable relocate this slot here.
   Leaving it inherited would point the slot at 0x0201493c. */
u32 dCcAcPos_c::GetOwnerID() {
    return dCcAc_c::GetOwnerID();
}

// @symbol _ZN7dCcAc_cC2Ev
dCcAc_c::dCcAc_c() : owner(0)
{
}

// @symbol _ZN7dCcAc_cD1Ev
dCcAc_c::~dCcAc_c()
{
}

// @symbol _ZN7dCcAc_c6GetPosEv
/* Slot 2. Returns the OWNER's position (dActor_c + 0x5c), not a field of this
   object -- a moving cylinder tracks its dActor_c instead of storing a copy. */
Vector3 &dCcAc_c::GetPos()
{
    return owner->mPos;
}

// @symbol _ZN7dCcAc_c10GetOwnerIDEv
/* Slot 3. owner->uniqueID, at dActor_c + 4 -- the same offset dBgW
   reads for its own ownerUniqueID. */
u32 dCcAc_c::GetOwnerID()
{
    return owner->uniqueID;
}

// @symbol _ZN7dCcAc_c4InitEP8dActor_ciijj
void dCcAc_c::Init(dActor_c *actor, Fix12i radius, Fix12i height, u32 flags, u32 vulnFlags) {
    owner = actor;
    dCc_c::Init(radius, height, flags, vulnFlags);
}

// @symbol _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector3iijj
void dCcAcPos_c::Init(dActor_c *actor, const Vector3 &offset, Fix12i radius, Fix12i height,
                      u32 flags, u32 vulnFlags) {
    dCcAc_c::Init(actor, radius, height, flags, vulnFlags);
    SetPosRelativeToActor(offset);
}

// @symbol _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3
/* Places this collider at an offset expressed in the owner's local frame:
   rotate the offset by the owner's yaw, then translate by the owner's
   position. dCcAcPos_c::Init calls it once at construction.

   mAngleY is s16 and the ROM's load is `ldrsh' -- reading it through the named
   member keeps the sign.
   local extern: no header declares Vec3_RotateYAndTranslate. */
extern "C" void Vec3_RotateYAndTranslate(Vector3 *res, const Vector3 *translation, s16 angY, const Vector3 *v);

void dCcAcPos_c::SetPosRelativeToActor(const Vector3 &offset)
{
    dActor_c *actor = owner;
    Vec3_RotateYAndTranslate(&pos, &actor->mPos, actor->mAngleY, &offset);
}
