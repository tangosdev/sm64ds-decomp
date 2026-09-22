//cpp
#include "types.h"
// @symbol _ZN10LavaBubble8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "LavaBubble.h"
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct dCc_c;
struct dBgCh_Actr;

extern "C" {
int _ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(void* self, Fix12i d);
void _ZN7fBase_c18MarkForDestructionEv(void* self);
unsigned short DecIfAbove0_Short(unsigned short* p);
void* _ZN8dActor_c10FindWithIDEj(unsigned int id);
void _ZN6Player4BurnEv(void* self);
void _ZN8dActor_c9UpdatePosEP5dCc_c(void* self, dCc_c* cc);
void _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(void* self, dBgCh_Actr* wm, unsigned int j);
void _ZN5dCc_c5ClearEv(dCc_c* self);
void _ZN5dCc_c6UpdateEv(dCc_c* self);
}

int LavaBubble::Behavior()
{
    int flags = mFlags;
    int b20 = (flags & 0x20000) != 0;

    if (b20) {
        M* m = *(M**)((char*)&unk_300);
        if (m->pmf != 0)
            (((Klass*)((char*)this))->*(m->pmf))();
        return 1;
    }

    {
        int b40 = (flags & 0x40000) != 0;
        if (b40)
            return 1;
    }

    if (_ZN8dActor_c22IsTooFarAwayFromPlayerE5Fix12IiE(((char*)this), 0x5dc000)) {
        if (unk_310 != 0)
            _ZN7fBase_c18MarkForDestructionEv(((char*)this));
        return 1;
    }

    DecIfAbove0_Short((unsigned short*)((char*)&mStateTimer));

    {
        unsigned int id = mdCcAc_c.otherOwner;
        if (id != 0) {
            if ((mdCcAc_c.hitFlags & 0x8000) == 0) {
                char* a = (char*)_ZN8dActor_c10FindWithIDEj(id);
                if (a != 0) {
                    int hit = (*(u16*)(a + 0xc) == 0xbf);
                    if (hit)
                        _ZN6Player4BurnEv(a);
                }
            } else {
                mdCcAc_c.flags |= 1;
            }
        }
    }

    {
        M* m = *(M**)((char*)&unk_300);
        if (m->pmf != 0)
            (((Klass*)((char*)this))->*(m->pmf))();
    }

    _ZN8dActor_c9UpdatePosEP5dCc_c(((char*)this), (dCc_c*)((char*)&mdCcAc_c));

    if (mVertAccel != 0)
        _ZN12dEnemyBase_c12UpdateWMClsnER10dBgCh_Actrj(((char*)this), (dBgCh_Actr*)((char*)&mWithMeshClsn), 0);

    _ZN5dCc_c5ClearEv((dCc_c*)((char*)&mdCcAc_c));
    _ZN5dCc_c6UpdateEv((dCc_c*)((char*)&mdCcAc_c));
    return 1;
}
