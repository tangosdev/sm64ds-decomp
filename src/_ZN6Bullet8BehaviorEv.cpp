//cpp
// @symbol _ZN6Bullet8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Bullet.h"
extern "C" unsigned short DecIfAbove0_Short(unsigned short* p);

struct CylinderClsn { int dummy; };
struct WithMeshClsn { int dummy; };

struct Holder;

struct Actor {
    char pad[0x350];
    Holder* h;
};

typedef void (Actor::*PMF)();

struct Holder {
    void* a0;
    void* a4;
    PMF pmf;   // function-pointer word at offset 8
};


struct CC {
    void Clear();
    void Update();
};

extern "C" void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(Actor* a, CylinderClsn* c);
extern "C" void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(Actor* a, WithMeshClsn* w, unsigned int n);
extern "C" void _ZN12CylinderClsn5ClearEv(void* c);
extern "C" void _ZN12CylinderClsn6UpdateEv(void* c);

int Bullet::Behavior()
{
    DecIfAbove0_Short((unsigned short*)((char*)&unk_100));
    Holder* h = ((Actor*)this)->h;
    if (h->pmf != 0) {
        (((Actor*)this)->*(h->pmf))();
    }
    {
        int spd = *(int*)((char*)&unk_0a8);
        int pos = *(int*)((char*)&unk_09c);
        int lim = *(int*)((char*)&unk_0a0);
        int ac = *(int*)((char*)&unk_0ac);
        int np = spd + pos;
        if (np >= lim) lim = np;
        *(int*)((char*)&unk_0a8) = lim;
        *(int*)((char*)&unk_0ac) = ac;
        _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(((Actor*)this), (CylinderClsn*)((char*)&mMovingCylinderClsn));
    }
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(((Actor*)this), (WithMeshClsn*)((char*)&mWithMeshClsn), 0);
    *(short*)((char*)&unk_08e) = *(short*)((char*)&unk_094);
    func_ov002_020fed7c((char*)((Actor*)this));
    _ZN12CylinderClsn5ClearEv((char*)&mMovingCylinderClsn);
    _ZN12CylinderClsn6UpdateEv((char*)&mMovingCylinderClsn);
    return 1;
}
