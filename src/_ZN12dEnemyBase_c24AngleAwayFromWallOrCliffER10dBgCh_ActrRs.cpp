//cpp
// @symbol _ZN12dEnemyBase_c24AngleAwayFromWallOrCliffER10dBgCh_ActrRs
/* recovered: named members + shared header, real C++ method
 *
 * On a wall, reflect the heading off it; on a cliff edge (unk_106), turn
 * around; otherwise report that nothing was done.
 *
 * The wall normal's X and Z are what ReflectAngle bounces the heading off.
 * dEnemyBase_c.h declared those slots u8 with padding between them; UpdateWMClsn
 * shows SurfaceInfo::CopyNormalTo writing a whole Vector3 there, so they are
 * three s32 and are named in this commit.
 */
#include "dEnemyBase_c.h"
extern "C" {
extern int _ZNK10dBgCh_Actr8IsOnWallEv(void *clsn);
/* ReflectAngle takes Fix12<int> by value -- the mwccarm 6az wall, runbook
   section 7 -- so it stays extern "C" with scalars in those slots. */
extern short _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(void *actor, int a, int b, short c);
}

int dEnemyBase_c::AngleAwayFromWallOrCliff(dBgCh_Actr & clsn_, short & outAngle_)
{
    void *clsn = &clsn_;
    short *outAngle = &outAngle_;
    if (_ZNK10dBgCh_Actr8IsOnWallEv(clsn)) {
        *outAngle = _ZN8dActor_c12ReflectAngleE5Fix12IiES1_s(this,
            mWallNormalX, mWallNormalZ, *outAngle);
    } else if (unk_106) {
        *outAngle = (short)(mPrevAngleY + 0x8000);
    } else {
        return 0;
    }
    return 1;
}
