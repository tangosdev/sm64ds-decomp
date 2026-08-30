//cpp
// @symbol _ZN7daDkk_c13InitResourcesEv
/* func_ov025_02111c24, 0xd0 bytes -- vtable slot 0, override of the
 * abstract slot daDsnBase_c leaves null.
 *
 * Stores the file table pointer at 0x320 (daDsnBase_c's own mFileTable),
 * calls a shared ov091 helper, then either forces mState (0x398) to a fixed
 * "airborne" value or runs a downward raycast from the actor's own position
 * (mPosX/mPosY/mPosZ, dActor_c's, already named there) to set mProbeHeight
 * (0x390) from the second collision point it finds. The flag test at +0x8
 * and the raycast machinery are read through raw offsets/opaque locals,
 * matching the un-migrated file this replaces -- neither belongs to this
 * class's own layout.
 */
#include "daDkk_c.h"
#include "dBgCh_Lin.h"

extern "C" {
extern int data_ov025_02113814[];
extern int func_ov091_02133254(void* c);
}

int daDkk_c::InitResources()
{
    char *c = (char *)this;
    *(void **)(c + 0x320) = data_ov025_02113814;
    int r = func_ov091_02133254(c);
    if (*(int *)(c + 8) & 1) {
        mState = 6;
    } else {
        mState = 0;
        dBgCh_Lin ray;
        Vector3 va;
        Vector3 vb;
        int x = *(int *)(c + 0x5c);
        vb.x = x;
        int y = *(int *)(c + 0x60);
        vb.y = y;
        int z = *(int *)(c + 0x64);
        va.x = x;
        vb.z = z;
        va.y = y;
        va.z = z;
        vb.y = y + 0x7d0000;
        ray.SetObjAndLine(va, vb, this);
        if (ray.DetectClsn()) {
            Vector3 p1 = ray.GetClsnPos();
            Vector3 p2 = ray.GetClsnPos();
            mProbeHeight = p2.y - 0x190000;
        }
    }
    return r;
}
