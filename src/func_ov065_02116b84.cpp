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
extern void func_ov065_0211696c(char *c);
extern int func_ov065_0211691c(void *c, void *p);
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(Enemy *thiz, WithMeshClsn *wm, ModelAnim *ma, unsigned int j);
extern int ApproachAngle(short *target, short from, short start, short speed, short max);
extern void _ZN5Enemy11UpdateDeathER12WithMeshClsn(Enemy *thiz, WithMeshClsn *wm);
extern unsigned short DecIfAbove0_Short(unsigned short *p);
extern void _Z14ApproachLinearRiii(int *x, int target, int step);
extern void _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(Enemy *thiz, void *clsn);
extern void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(Enemy *thiz, WithMeshClsn *wm, unsigned int j);
extern void func_ov065_02115ff0(char *c);
extern char *_ZN5Actor13ClosestPlayerEv(Enemy *thiz);
extern void _ZN9Animation7AdvanceEv(void *thiz);

extern short SINE_TABLE[];
extern char data_ov065_0211d650[];
extern char data_ov065_0211d660[];
extern int data_ov065_0211d670[];
}

struct Enemy { char pad[0x800]; };

struct ActorOv065 {
    char pad[0x3d8];
    int ang;
};

extern "C" int func_ov065_02116b84(Enemy *thiz)
{
    char *c = (char *)thiz;
    if (_ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(thiz, (WithMeshClsn *)(c + 0x144)) != 0) {
        _ZN12CylinderClsn5ClearEv(c + 0x110);
        if (*(unsigned char *)(c + 0x107) != 0) {
            if (*(unsigned short *)(c + 0x104) == 0) {
                _ZN12CylinderClsn6UpdateEv(c + 0x110);
            }
        }
        func_ov065_0211696c(c);
        *(int *)(c + 0x3cc) = *(int *)(c + 0x5c);
        *(int *)(c + 0x3d0) = *(int *)(c + 0x60);
        *(int *)(c + 0x3d4) = *(int *)(c + 0x64);
        func_ov065_0211691c(c, data_ov065_0211d670);
        return 1;
    }
    if (_ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(thiz, (WithMeshClsn *)(c + 0x144), (ModelAnim *)(c + 0x300), 3) != 0) {
        return 1;
    }
    if (*(int *)(c + 0x10c) != 0) {
        ApproachAngle((short *)(c + 0x8c), -0x4000, 0xa, 0x200, 0x100);
        _ZN5Enemy11UpdateDeathER12WithMeshClsn(thiz, (WithMeshClsn *)(c + 0x144));
        func_ov065_0211696c(c);
        return 1;
    }
    DecIfAbove0_Short((unsigned short *)(c + 0x100));
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
    if (*(char **)(c + 0x3bc) != data_ov065_0211d650) {
        int *p3d8;
        int ang;
        int idx;
        short tbl;
        int result;
        p3d8 = (int *)(((int)c + 0x3d8) & 0xFFFFFFFFFFFFFFFF);
        *p3d8 += 0x200;
        ang = *(int *)(c + 0x3d8);
        idx = ((unsigned short)(short)ang >> 4) * 2;
        tbl = SINE_TABLE[idx];
        result = (int)(((long long)tbl * 0x46000 + 0x800) >> 12);
        _Z14ApproachLinearRiii((int *)(void *)(c + 0x60), *(int *)(c + 0x3d0) + (result + 0xb4000), 0x3000);
    }
    _ZN5Actor22UpdatePosWithOnlySpeedEP12CylinderClsn(thiz, (void *)(c + 0x110));
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(thiz, (WithMeshClsn *)(c + 0x144), 0);
    func_ov065_0211696c(c);
    if (*(char **)(c + 0x3bc) != data_ov065_0211d660) {
        *(short *)(c + 0x8c) = *(short *)(c + 0x92);
        *(short *)(c + 0x8e) = *(short *)(c + 0x94);
        *(short *)(c + 0x90) = *(short *)(c + 0x96);
        func_ov065_02115ff0(c);
    }
    _ZN12CylinderClsn5ClearEv(c + 0x110);
    {
        char *p = _ZN5Actor13ClosestPlayerEv(thiz);
        if (p != 0 && *(unsigned char *)(p + 0x6fb) == 0) {
            _ZN12CylinderClsn6UpdateEv(c + 0x110);
        }
    }
    _ZN9Animation7AdvanceEv(c + 0x350);
    return 1;
}
