//cpp
// @symbol _ZN12dEnemyBase_c27SpawnParticlesIfHitOtherObjER5dCc_c
/* recovered: named members + shared header, real C++ method
 *
 * While mEatenByYoshi is set, a cylinder collision against anything other than actor
 * IDs 0x120/0x121 spawns the mega-character particles; otherwise bit 0x20000
 * on the collision is raised. Clearing mEatenByYoshi clears that bit instead.
 *
 * The clsn offsets stay raw: dCc_c has no header here, and +0x18 and
 * +0x24 are the only two slots this function evidences.
 */
#include "dEnemyBase_c.h"
extern "C" {
extern void* _ZN8dActor_c10FindWithIDEj(unsigned int);
extern void _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc(void* self, void* a, char* p);
}

int dEnemyBase_c::SpawnParticlesIfHitOtherObj(dCc_c & clsn_)
{
    char* clsn = (char*)&clsn_;
    int* f;
    if (mEatenByYoshi != 0) {
        unsigned int id = *(unsigned int*)(clsn+0x24);
        if (id != 0) {
            void* a = _ZN8dActor_c10FindWithIDEj(id);
            if (a != 0) {
                unsigned short t = *(unsigned short*)((char*)a+0xc);
                int e1 = (t == 0x120);
                if (e1 == 0) {
                    int e2 = (t == 0x121);
                    if (e2 == 0) {
                        _ZN12dEnemyBase_c22SpawnMegaCharParticlesER8dActor_cPc(this, a, clsn);
                        return 1;
                    }
                }
            }
        }
        f = (int*)(((int)clsn + 0x18));
        *f = *f | 0x20000;
        goto done;
    }
    f = (int*)(((int)clsn + 0x18));
    *f = *f & ~0x20000;
done:
    return 0;
}
