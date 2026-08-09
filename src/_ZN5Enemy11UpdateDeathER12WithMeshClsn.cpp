//cpp
// @symbol _ZN5Enemy11UpdateDeathER12WithMeshClsn
/* recovered: named members + shared header, real C++ method
 *
 * Runs one frame of the death animation. unk_10c selects which death handler
 * from a table of POINTERS TO MEMBER FUNCTION -- the third such table found in
 * this tree, after Player::State and Player's level-entry steps -- then the
 * position and mesh collision are updated regardless.
 *
 * The table was declared through a fake `struct Enemy { char pad[0x200]; }`
 * that existed only to hang the pmf type off; the real class serves now.
 */
#include "Enemy.h"

extern int (Enemy::*data_ov002_0210dbc0[])(WithMeshClsn &);

extern "C" {
extern void DecIfAbove0_Short(short *p);
extern int _ZN5Actor9UpdatePosEP12CylinderClsn(void *thiz, void *clsn);
extern int _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *thiz, WithMeshClsn *clsn, u32 sel);
}

int Enemy::UpdateDeath(WithMeshClsn & clsn_)
{
    WithMeshClsn *clsn = &clsn_;
    int ret;
    if (unk_10c == 0)
        return 0;
    DecIfAbove0_Short(&unk_102);
    ret = (this->*data_ov002_0210dbc0[unk_10c - 1])(*clsn);
    _ZN5Actor9UpdatePosEP12CylinderClsn(this, 0);
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(this, clsn, 0);
    return ret;
}
