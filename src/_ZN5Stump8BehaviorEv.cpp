//cpp
struct WithMeshClsn;
struct CylinderClsn;
struct Enemy;
typedef void (Enemy::*PMF)();
struct Holder { char pad[8]; PMF fn; };

extern "C" {
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(Enemy *thiz, WithMeshClsn *c);
extern void _ZN12CylinderClsn5ClearEv(void *thiz);
extern void _ZN12CylinderClsn6UpdateEv(void *thiz);
extern void func_ov091_02134094(char *c);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(Enemy *thiz, void *clsn);
extern void func_ov091_021339fc(char *thiz);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(Enemy *thiz, WithMeshClsn *wm, unsigned int j);
extern void _ZN9Animation7AdvanceEv(void *thiz);
}

struct Enemy { char pad[0x800]; };

extern "C" int _ZN5Stump8BehaviorEv(Enemy *thiz)
{
    char *c = (char *)thiz;
    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(thiz, (WithMeshClsn *)(c + 0x144)) != 0) {
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        if (*(unsigned char *)(c + 0x107) != 0) {
            if (*(unsigned short *)(c + 0x104) == 0) {
                _ZN12CylinderClsn6UpdateEv(c + 0x110);
            }
        }
        func_ov091_02134094(c);
        return 1;
    }

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    {
        Holder *q = *(Holder **)(c + 0x364);
        if (q->fn != 0) (thiz->*(q->fn))();
    }
    *(short *)(c + 0x8c) = *(short *)(c + 0x92);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(short *)(c + 0x90) = *(short *)(c + 0x96);
    {
        int v = *(int *)(c + 0xa8) + *(int *)(c + 0x9c);
        int hi = *(int *)(c + 0xa0);
        if (v >= hi)
            hi = v;
        int tmp = *(int *)(c + 0xac);
        *(int *)(c + 0xa8) = hi;
        *(int *)(c + 0xac) = tmp;
    }
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(thiz, (void *)(c + 0x110));
    func_ov091_021339fc(c);
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    _ZN12CylinderClsn6UpdateEv(c + 0x110);

    if (*(int *)(c + 0x374) == 1) {
        _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(thiz, (WithMeshClsn *)(c + 0x144), 0);
        return 1;
    }

    _ZN9Animation7AdvanceEv(c + 0x350);
    func_ov091_02134094(c);
    return 1;
}
