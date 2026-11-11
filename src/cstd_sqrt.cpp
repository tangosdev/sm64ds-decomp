//cpp
#include "types.h"
#include "nitro/cp/sqrt.h"

/* sqrt(x), rounded to nearest: the hardware root of x * 4, halved. VEC_Mag's
   tail on a 64-bit square the caller already has. */

namespace cstd {

int sqrt(unsigned long long x)
{
    CP_SetSqrt64(x << 2);
    return ((s32)CP_GetSqrtResult32() + 1) >> 1;
}

}

/* Zero angles copied by the mascot walker and the ? block. File-local so the
   registration can alias the existing arm9 destructor. */
#ifndef SM64DS_PLATFORM_PC
struct ZeroVector3s {
    short x;
    short y;
    short z;

    ZeroVector3s() : x(0), y(0), z(0) {}
    ~ZeroVector3s();
};

ZeroVector3s data_020a0edc;
#endif
