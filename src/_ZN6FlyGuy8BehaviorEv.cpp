//cpp
struct WithMeshClsn;
struct Enemy;
typedef void (Enemy::*PMF)();
struct Holder { char pad[8]; PMF fn; };

extern "C" {
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(Enemy *thiz, WithMeshClsn *c);
extern void _ZN12CylinderClsn5ClearEv(void *thiz);
extern void _ZN12CylinderClsn6UpdateEv(void *thiz);
extern void func_ov070_02120070(char *c);
extern void _ZN5Enemy11UpdateDeathER12WithMeshClsn(Enemy *thiz, WithMeshClsn *wm);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(Enemy *thiz, void *clsn);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(Enemy *thiz, WithMeshClsn *wm, unsigned int j);
extern void func_ov070_0211f100(char *c);
extern char *_ZN5Actor13ClosestPlayerEv(Enemy *thiz);
extern void _ZN9Animation7AdvanceEv(void *thiz);

extern char data_ov070_021235cc[];
extern char data_ov070_021235bc[];
}

struct Enemy { char pad[0x800]; };

extern "C" int _ZN6FlyGuy8BehaviorEv(Enemy *thiz)
{
    char *c = (char *)thiz;

    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(thiz, (WithMeshClsn *)(c + 0x144)) != 0) {
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        if (*(unsigned char *)(c + 0x107) != 0) {
            if (*(unsigned short *)(c + 0x104) == 0) {
                _ZN12CylinderClsn6UpdateEv(c + 0x110);
            }
        }
        func_ov070_02120070(c);
        return 1;
    }

    if (*(int *)(c + 0x10c) != 0) {
        _ZN5Enemy11UpdateDeathER12WithMeshClsn(thiz, (WithMeshClsn *)(c + 0x144));
        func_ov070_02120070(c);
        return 1;
    }

    if (*(char **)(c + 0x3bc) != data_ov070_021235cc) {
        DecIfAbove0_Short((unsigned short *)(c + 0x100));
    }
    DecIfAbove0_Short((unsigned short *)(c + 0x3cc));

    {
        Holder *q = *(Holder **)(c + 0x3bc);
        if (q->fn != 0) {
            (thiz->*(q->fn))();
        }
    }

    {
        int v = *(int *)(c + 0xa8) + *(int *)(c + 0x9c);
        int hi = *(int *)(c + 0xa0);
        if (v >= hi) {
            hi = v;
        }
        int tmp = *(int *)(c + 0xac);
        *(int *)(c + 0xa8) = hi;
        *(int *)(c + 0xac) = tmp;
    }

    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(thiz, (void *)(c + 0x110));
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(thiz, (WithMeshClsn *)(c + 0x144), 0);

    if (*(char **)(c + 0x3bc) != data_ov070_021235bc) {
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        *(short *)(c + 0x90) = *(short *)(c + 0x96);
    }

    func_ov070_02120070(c);

    if (*(char **)(c + 0x3bc) != data_ov070_021235bc) {
        func_ov070_0211f100(c);
    }

    _ZN12CylinderClsn5ClearEv(c + 0x110);
    {
        char *p = _ZN5Actor13ClosestPlayerEv(thiz);
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0) {
            _ZN12CylinderClsn6UpdateEv(c + 0x110);
        }
    }

    *(int *)(c + 0x35c) = 0x1000;
    _ZN9Animation7AdvanceEv(c + 0x350);
    return 1;
}
