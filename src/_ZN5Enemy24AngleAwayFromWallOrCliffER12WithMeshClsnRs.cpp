//cpp
// @symbol _ZN5Enemy24AngleAwayFromWallOrCliffER12WithMeshClsnRs
/* recovered: named members + shared header, real C++ method
 *
 * On a wall, reflect the heading off it; on a cliff edge (unk_106), turn
 * around; otherwise report that nothing was done.
 *
 * The wall normal's X and Z are what ReflectAngle bounces the heading off.
 * Enemy.h declared those slots u8 with padding between them; UpdateWMClsn
 * shows SurfaceInfo::CopyNormalTo writing a whole Vector3 there, so they are
 * three s32 and are named in this commit.
 */
#include "Enemy.h"
extern "C" {
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *clsn);
/* ReflectAngle takes Fix12<int> by value -- the mwccarm 6az wall, runbook
   section 7 -- so it stays extern "C" with scalars in those slots. */
extern short _ZN5Actor12ReflectAngleE5Fix12IiES1_s(void *actor, int a, int b, short c);
}

int Enemy::AngleAwayFromWallOrCliff(WithMeshClsn & clsn_, short & outAngle_)
{
    void *clsn = &clsn_;
    short *outAngle = &outAngle_;
    if (_ZNK12WithMeshClsn8IsOnWallEv(clsn)) {
        *outAngle = _ZN5Actor12ReflectAngleE5Fix12IiES1_s(this,
            mWallNormalX, mWallNormalZ, *outAngle);
    } else if (unk_106) {
        *outAngle = (short)(unk_094 + 0x8000);
    } else {
        return 0;
    }
    return 1;
}
