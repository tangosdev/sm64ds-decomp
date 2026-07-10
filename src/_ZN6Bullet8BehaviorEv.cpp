//cpp
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

extern "C" void func_ov002_020fed7c(char* c);

struct CC {
    void Clear();
    void Update();
};

extern "C" void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(Actor* a, CylinderClsn* c);
extern "C" void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(Actor* a, WithMeshClsn* w, unsigned int n);
extern "C" void _ZN12CylinderClsn5ClearEv(void* c);
extern "C" void _ZN12CylinderClsn6UpdateEv(void* c);

extern "C" int _ZN6Bullet8BehaviorEv(Actor* a) {
    DecIfAbove0_Short((unsigned short*)((char*)a + 0x100));
    Holder* h = a->h;
    if (h->pmf != 0) {
        (a->*(h->pmf))();
    }
    {
        int spd = *(int*)((char*)a + 0xa8);
        int pos = *(int*)((char*)a + 0x9c);
        int lim = *(int*)((char*)a + 0xa0);
        int ac = *(int*)((char*)a + 0xac);
        int np = spd + pos;
        if (np >= lim) lim = np;
        *(int*)((char*)a + 0xa8) = lim;
        *(int*)((char*)a + 0xac) = ac;
        _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(a, (CylinderClsn*)((char*)a + 0x110));
    }
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(a, (WithMeshClsn*)((char*)a + 0x144), 0);
    *(short*)((char*)a + 0x8e) = *(short*)((char*)a + 0x94);
    func_ov002_020fed7c((char*)a);
    _ZN12CylinderClsn5ClearEv((char*)a + 0x110);
    _ZN12CylinderClsn6UpdateEv((char*)a + 0x110);
    return 1;
}
