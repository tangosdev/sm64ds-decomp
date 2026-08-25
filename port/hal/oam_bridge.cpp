// Historical raw-name calls in the OAM render TU -> real migrated static
// methods. Static methods are cdecl, but typed wrappers keep the signatures
// reviewable and avoid hard-coding MSVC decorations.
#include "OAM.h"
#include "OamAttr.h"

extern "C" {
void _ZN3OAM4LoadEv() { OAM::Load(); }
u8 _ZN3OAM11GetObjWidthEii(int shape, int size)
{ return OAM::GetObjWidth(shape, size); }
u8 _ZN3OAM12GetObjHeightEii(int shape, int size)
{ return OAM::GetObjHeight(shape, size); }
int _ZN3OAM16LoadAffineParamsEP7OamAttrPiP9Matrix2x2(
    OamAttr *oam, int *count, Matrix2x2 *matrix)
{ return OAM::LoadAffineParams(oam, count, matrix); }
}
