//cpp
// @symbol _ZN5Enemy27SpawnParticlesIfHitOtherObjER12CylinderClsn
/* recovered: named members + shared header, real C++ method
 *
 * While unk_107 is set, a cylinder collision against anything other than actor
 * IDs 0x120/0x121 spawns the mega-character particles; otherwise bit 0x20000
 * on the collision is raised. Clearing unk_107 clears that bit instead.
 *
 * The clsn offsets stay raw: CylinderClsn has no header here, and +0x18 and
 * +0x24 are the only two slots this function evidences.
 */
#include "Enemy.h"
extern "C" {
extern void* _ZN5Actor10FindWithIDEj(unsigned int);
extern void _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(void* self, void* a, char* p);
}

int Enemy::SpawnParticlesIfHitOtherObj(CylinderClsn & clsn_)
{
    char* clsn = (char*)&clsn_;
    int* f;
    if (unk_107 != 0) {
        unsigned int id = *(unsigned int*)(clsn+0x24);
        if (id != 0) {
            void* a = _ZN5Actor10FindWithIDEj(id);
            if (a != 0) {
                unsigned short t = *(unsigned short*)((char*)a+0xc);
                int e1 = (t == 0x120);
                if (e1 == 0) {
                    int e2 = (t == 0x121);
                    if (e2 == 0) {
                        _ZN5Enemy22SpawnMegaCharParticlesER5ActorPc(this, a, clsn);
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
