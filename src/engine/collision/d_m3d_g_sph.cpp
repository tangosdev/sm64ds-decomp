//cpp
// @symbol _ZNK8dM3dGSph9GetCentreER7Vector3
// @symbol _ZN8dM3dGSph9SetRadiusEi
// @symbol _ZN8dM3dGSph3SetERK7Vector3i
// @symbol _ZN8dM3dGSph9SetCentreERK7Vector3
// @symbol _ZN8dM3dGSphD2Ev
// @symbol _ZN8dM3dGSphD0Ev
// @symbol _ZN8dM3dGSphD1Ev
// @symbol _ZN8dM3dGSphC1Ev
// @symbol _ZN8dM3dGSphC2Ev
/* dM3dGSph -- the cartridge's sphere primitive: a centre, a radius and a
 * vtable, 0x14 bytes. ROM's own RTTI name (_ZTS8dM3dGSph @ 0x020992ec).
 * Lives as a base of dBgCh_SphCrr at 0x38 (the query sphere) and as
 * dBgCh_Lin's bounding-sphere member at 0x64.
 *
 * Deferred codegen emits each lifecycle group in its fixed order --
 * D2,D0,D1 for the destructor and C1,C2 for the constructor -- and the
 * plain members land in reverse definition order, so the file defines
 * ctor and dtor first. All five variants are cartridge-retained here:
 * dBgCh_SphCrr's MI construction calls C2/D2, dBgCh_Lin's member calls
 * C1/D1.
 *
 * The vtable/RTTI are emitted (the destructor is the key function) and
 * licensed deadstrip-data against their canonical homes. The four field
 * accessors were enrolled as anonymous func_ shards; they are real
 * members now, and symbols.txt names them accordingly.
 *
 * leftovers: none.
 */
#include "dM3dGSph.h"

dM3dGSph::dM3dGSph() {}

dM3dGSph::~dM3dGSph() {}

void dM3dGSph::SetCentre(const Vector3 &centre_) {
    centre = centre_;
}

void dM3dGSph::Set(const Vector3 &centre_, Fix12i radius_) {
    SetCentre(centre_);
    SetRadius(radius_);
}

void dM3dGSph::SetRadius(Fix12i radius_) {
    radius = radius_;
}

void dM3dGSph::GetCentre(Vector3 &out) const {
    out = centre;
}
