//cpp
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct CylinderClsn;
struct WithMeshClsn;
extern "C" unsigned short DecIfAbove0_Short(unsigned short *p);
extern "C" void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(void *self, CylinderClsn *cc);
extern "C" void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, WithMeshClsn *wm, unsigned int j);
extern "C" void func_ov081_02126224(char *c);
extern "C" void _ZN12CylinderClsn5ClearEv(CylinderClsn *self);
extern "C" void *_ZN5Actor13ClosestPlayerEv(void *self);
extern "C" void _ZN12CylinderClsn6UpdateEv(CylinderClsn *self);

extern "C" int _ZN8Snowball8BehaviorEv(char *c)
{
    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    M *m = *(M **)(c + 0x378);
    if (m->pmf != 0)
        (((Klass *)c)->*(m->pmf))();
    int v = *(int *)(c + 0xa8) + *(int *)(c + 0x9c);
    int hi = *(int *)(c + 0xa0);
    if (v >= hi)
        hi = v;
    int tmp = *(int *)(c + 0xac);
    *(int *)(c + 0xa8) = hi;
    *(int *)(c + 0xac) = tmp;
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(c, (CylinderClsn *)(c + 0x110));
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, (WithMeshClsn *)(c + 0x144), 0);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    func_ov081_02126224(c);
    _ZN12CylinderClsn5ClearEv((CylinderClsn *)(c + 0x110));
    void *p = _ZN5Actor13ClosestPlayerEv(c);
    if (p != 0 && *(unsigned char *)((char *)p + 0x6fb) == 0)
        _ZN12CylinderClsn6UpdateEv((CylinderClsn *)(c + 0x110));
    return 1;
}
