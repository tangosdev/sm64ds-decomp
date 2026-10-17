//cpp
/* leftovers:
 * func_02014a20 stays a free helper under its address name: the cartridge
 * record carries the linker's tail-call veneer to dCcAc_c::GetOwnerID, not a
 * member body (dCcAcPos_c.h slot-3 note).
 * The three Init definitions stay mangled free functions: the real signatures
 * carry Fix12<int> by value (wall 6az); dCcAc_c.h and dCcAcPos_c.h keep the
 * real declarations for callers.
 */
/* dCcAc_c -- collision cylinder attached to an dActor_c. Cartridge class:
   _ZTV7dCcAc_c at 0x0208e6d4, _ZTS7dCcAc_c at 0x0208e668, _ZTI7dCcAc_c at
   0x0208e698 (si_class, base dCc_c). The derived position-bearing class
   dCcAcPos_c (_ZTV10dCcAcPos_c at 0x0208e704, si_class on this class) folds
   into the same TU for its non-lifecycle members: the run brackets the
   base span (GetPos and the slot-3 veneer above it, Init and
   SetPosRelativeToActor below it), while D0/D1/C1 stay in their own files
   because defining the key function here would re-emit _ZTV10dCcAcPos_c
   with slot 3 bound to the base member, not to the linker veneer the
   cartridge record carries. TU claims 0x0201489c..0x02014a34. Written
   back-to-front: mwccarm emits .text in reverse source order under default
   deferred codegen. */
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "dActor_c.h"
#include "decl_dCcAc_c.h"

// @symbol _ZN10dCcAcPos_c6GetPosEv
/* Vtable slot 2. This shadows dCcAc_c's own slot 2, which returns the
   OWNER's position; this class carries its own. */
Vector3 &dCcAcPos_c::GetPos()
{
    return pos;
}

extern "C" {

// @symbol func_02014a20
/* dCcAcPos_c vtable slot 3: the compiler emitted a 12-byte interworking
   veneer tail-calling the inherited dCcAc_c::GetOwnerID instead of a real
   override body. */
void func_02014a20(void) {
    _ZN7dCcAc_c10GetOwnerIDEv();
}

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

// @symbol _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj
/* Stays a mangled free definition: the real signature carries Fix12<int> and
   wall 6az (notes/mwccarm-codegen.md) homes class-typed by-value parameters.
   The declaration in dCcAc_c.h is the real one and callers may use it. */
extern "C" void _ZN5dCc_c4InitE5Fix12IiES1_jj(dCc_c *self, int radius, int height, u32 flags, u32 vulnFlags);

extern "C" void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(dCcAc_c *self, dActor_c *actor, int radius, int height, u32 flags, u32 vulnFlags)
{
    self->owner = actor;
    _ZN5dCc_c4InitE5Fix12IiES1_jj(self, radius, height, flags, vulnFlags);
}

// @symbol _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj
/* Stays a mangled free definition like the base Init it wraps: the real
   signature carries Fix12<int> by value, same wall 6az. The declaration in
   dCcAcPos_c.h is the real one and callers may use it. */
extern "C" void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(dCcAcPos_c *self, dActor_c *actor, const Vector3 *offset, int radius, int height, u32 flags, u32 vulnFlags)
{
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(self, actor, radius, height, flags, vulnFlags);
    self->SetPosRelativeToActor(*offset);
}

// @symbol _ZN10dCcAcPos_c21SetPosRelativeToActorERK7Vector3
/* Places this collider at an offset expressed in the owner's local frame:
   rotate the offset by the owner's yaw, then translate by the owner's
   position. dCcAcPos_c::Init calls it once at construction.

   mAngleY is s16 and the ROM's load is `ldrsh' -- reading it through the named
   member keeps the sign. */
extern "C" void Vec3_RotateYAndTranslate(Vector3 *res, const Vector3 *translation, s16 angY, const Vector3 *v);

void dCcAcPos_c::SetPosRelativeToActor(const Vector3 &offset)
{
    dActor_c *actor = owner;
    Vec3_RotateYAndTranslate(&pos, &actor->mPos, actor->mAngleY, &offset);
}
