//cpp
struct WithMeshClsn;
struct ModelAnim;
struct Enemy;
typedef void (Enemy::*PMF)();
struct Holder { char pad[8]; PMF fn; };

extern "C" {
extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(Enemy *thiz, WithMeshClsn *c);
extern void _ZN12CylinderClsn5ClearEv(void *thiz);
extern void _ZN12CylinderClsn6UpdateEv(void *thiz);
extern void func_ov065_02117994(char *c);
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(Enemy *thiz, WithMeshClsn *wm, ModelAnim *ma, unsigned int j);
extern void _ZN5Enemy11UpdateDeathER12WithMeshClsn(Enemy *thiz, WithMeshClsn *wm);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern int _ZNK9Animation12WillHitFrameEi(void *thiz, int frame);
extern void func_02012694(int, void *);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(Enemy *thiz, void *clsn);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(Enemy *thiz, WithMeshClsn *wm, unsigned int j);
extern void func_ov065_0211704c(char *c);
extern char *_ZN5Actor13ClosestPlayerEv(Enemy *thiz);
extern void _ZN9Animation7AdvanceEv(void *thiz);

extern char data_ov065_0211d6e0[];
extern char data_ov065_0211d6f0[];
}

struct Enemy { char pad[0x800]; };

extern "C" int _ZN5Swoop8BehaviorEv(Enemy *thiz)
{
    char *c = (char *)thiz;
    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(thiz, (WithMeshClsn *)(c + 0x144)) != 0) {
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        if (*(unsigned char *)(c + 0x107) != 0) {
            if (*(unsigned short *)(c + 0x104) == 0) {
                _ZN12CylinderClsn6UpdateEv(c + 0x110);
            }
        }
        func_ov065_02117994(c);
        return 1;
    }
    if (_ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(thiz, (WithMeshClsn *)(c + 0x144), (ModelAnim *)(c + 0x300), 3) != 0) {
        return 1;
    }
    if (*(int *)(c + 0x10c) != 0) {
        _ZN5Enemy11UpdateDeathER12WithMeshClsn(thiz, (WithMeshClsn *)(c + 0x144));
        func_ov065_02117994(c);
        return 1;
    }
    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    {
        Holder *q = *(Holder **)(c + 0x420);
        if (q->fn != 0) (thiz->*(q->fn))();
    }
    {
        char *m = *(char **)(c + 0x420);
        if (m == data_ov065_0211d6e0 || m == data_ov065_0211d6f0) {
            if (_ZNK9Animation12WillHitFrameEi(c + 0x350, 3) != 0 ||
                _ZNK9Animation12WillHitFrameEi(c + 0x350, 0xf) != 0 ||
                _ZNK9Animation12WillHitFrameEi(c + 0x350, 0x1b) != 0) {
                func_02012694(0xe1, c + 0x74);
            }
        }
    }
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
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(thiz, (WithMeshClsn *)(c + 0x144), 0);
    *(short *)(c + 0x8c) = *(short *)(c + 0x92);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(short *)(c + 0x90) = *(short *)(c + 0x96);
    func_ov065_02117994(c);
    if (*(unsigned char *)(c + 0x43c) == 1) {
        func_ov065_0211704c(c);
    }
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    {
        char *p = _ZN5Actor13ClosestPlayerEv(thiz);
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0) {
            _ZN12CylinderClsn6UpdateEv(c + 0x110);
        }
    }
    if (*(unsigned char *)(c + 0x43c) == 1) {
        _ZN9Animation7AdvanceEv(c + 0x350);
    } else {
        _ZN9Animation7AdvanceEv(c + 0x3b4);
    }
    return 1;
}
