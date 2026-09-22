//cpp
// @symbol _ZN25RotatingUpDownPlatformUtm4KillEv
/* recovered: real C++ override -- slot 31, attributed by the vtable.
   config/arm9/overlays/ov091/relocs.txt: 0x02134cd8 -> 0x02131070, and
   _ZTV25RotatingUpDownPlatformUtm + 4*31 = 0x02134cd8. NOT the key function:
   the destructor is declared out of line and stays first (see the header),
   so this TU does not re-emit _ZTV25RotatingUpDownPlatformUtm -- checked
   with objisolate, not assumed. */
#include "RotatingUpDownPlatformUtm.h"

namespace Particle {
struct System { static System* NewSimple(unsigned int, int, int, int); };
/* Signature deliberately copied from the local declaration above: the ROM
   name carries by-value class parameters (e.g. Fix12<int>), which mwccarm
   passes differently at the call site, so declaring the true types breaks
   the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" System* _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
}
namespace Sound { void PlayBank3(unsigned int, const Vector3&); }

void RotatingUpDownPlatformUtm::Kill()
{
    Particle::_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xbb, mPosX, mPosY, mPosZ);
    Vector3 v = { mPosX, mPosY, mPosZ };
    PoofDustAt(v);
    Sound::PlayBank3(0xf, *(Vector3*)&mCamSpacePosX);
    mIsDead = 1;
    unk_31c = 0;
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
}
