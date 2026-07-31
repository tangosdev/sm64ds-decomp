//cpp
#include "types.h"
// @symbol _ZN10LavaBubble8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "LavaBubble.h"
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct CylinderClsn;
struct WithMeshClsn;

extern "C" {
int _ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(void* self, Fix12 d);
void _ZN9ActorBase18MarkForDestructionEv(void* self);
unsigned short DecIfAbove0_Short(unsigned short* p);
void* _ZN5Actor10FindWithIDEj(unsigned int id);
void _ZN6Player4BurnEv(void* self);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void* self, CylinderClsn* cc);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void* self, WithMeshClsn* wm, unsigned int j);
void _ZN12CylinderClsn5ClearEv(CylinderClsn* self);
void _ZN12CylinderClsn6UpdateEv(CylinderClsn* self);
}

int LavaBubble::Behavior()
{
    int flags = unk_0b0;
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

    if (_ZN5Actor22IsTooFarAwayFromPlayerE5Fix12IiE(((char*)this), 0x5dc000)) {
        if (unk_310 != 0)
            _ZN9ActorBase18MarkForDestructionEv(((char*)this));
        return 1;
    }

    DecIfAbove0_Short((unsigned short*)((char*)&unk_100));

    {
        unsigned int id = unk_134;
        if (id != 0) {
            if ((unk_130 & 0x8000) == 0) {
                char* a = (char*)_ZN5Actor10FindWithIDEj(id);
                if (a != 0) {
                    int hit = (*(u16*)(a + 0xc) == 0xbf);
                    if (hit)
                        _ZN6Player4BurnEv(a);
                }
            } else {
                *(int*)(((long long)(int)((char*)&unk_128))) |= 1;
            }
        }
    }

    {
        M* m = *(M**)((char*)&unk_300);
        if (m->pmf != 0)
            (((Klass*)((char*)this))->*(m->pmf))();
    }

    _ZN5Actor9UpdatePosEP12CylinderClsn(((char*)this), (CylinderClsn*)((char*)&mMovingCylinderClsn));

    if (unk_09c != 0)
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((char*)this), (WithMeshClsn*)((char*)&mWithMeshClsn), 0);

    _ZN12CylinderClsn5ClearEv((CylinderClsn*)((char*)&mMovingCylinderClsn));
    _ZN12CylinderClsn6UpdateEv((CylinderClsn*)((char*)&mMovingCylinderClsn));
    return 1;
}
