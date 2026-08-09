//cpp
// @symbol _ZN5Enemy24AngleAwayFromWallOrCliffER12WithMeshClsnRs
/* recovered: named members + shared header, real C++ method
 *
 * On a wall, reflect the heading off it; on a cliff edge (unk_106), turn
 * around; otherwise report that nothing was done.
 *
 * unk_0e0 and unk_0e8 are read through an int cast on purpose: Enemy.h
 * declares both u8, but this is the only Enemy code that touches them and it
 * reads four bytes from each -- they are the Fix12 pair ReflectAngle takes.
 * Not corrected in the header here because one reader is thin evidence for a
 * width, and the surrounding padding (pad_0e1[0x7]) would have to be resolved
 * with it.
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
            *(int*)&unk_0e0, *(int*)&unk_0e8, *outAngle);
    } else if (unk_106) {
        *outAngle = (short)(unk_094 + 0x8000);
    } else {
        return 0;
    }
    return 1;
}
