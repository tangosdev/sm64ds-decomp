//cpp
// @symbol _ZN12dEnemyBase_c11UpdateDeathER12WithMeshClsn
/* recovered: named members + shared header, real C++ method
 *
 * Runs one frame of the death animation. unk_10c selects which death handler
 * from a table of POINTERS TO MEMBER FUNCTION -- the third such table found in
 * this tree, after Player::State and Player's level-entry steps -- then the
 * position and mesh collision are updated regardless.
 *
 * The table was declared through a fake `struct dEnemyBase_c { char pad[0x200]; }`
 * that existed only to hang the pmf type off; the real class serves now.
 */
#include "dEnemyBase_c.h"

extern int (dEnemyBase_c::*data_ov002_0210dbc0[])(WithMeshClsn &);

extern "C" {
extern void DecIfAbove0_Short(unsigned short *p);
extern int _ZN8dActor_c9UpdatePosEP12CylinderClsn(void *thiz, void *clsn);
extern int _ZN12dEnemyBase_c12UpdateWMClsnER12WithMeshClsnj(void *thiz, WithMeshClsn *clsn, u32 sel);
}

int dEnemyBase_c::UpdateDeath(WithMeshClsn & clsn_)
{
    WithMeshClsn *clsn = &clsn_;
    int ret;
    if (mDeathState == 0)
        return 0;
    DecIfAbove0_Short(&mDeathTimer);
    ret = (this->*data_ov002_0210dbc0[mDeathState - 1])(*clsn);
    _ZN8dActor_c9UpdatePosEP12CylinderClsn(this, 0);
    _ZN12dEnemyBase_c12UpdateWMClsnER12WithMeshClsnj(this, clsn, 0);
    return ret;
}
