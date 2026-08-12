//cpp
struct Klass; typedef void (Klass::*PMF)();
struct M { char pad[8]; PMF pmf; };
struct CylinderClsn;
struct WithMeshClsn;
struct ModelAnim;
extern "C" {
int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void *self, WithMeshClsn *wm, ModelAnim *ma, unsigned int j);
unsigned short DecIfAbove0_Short(unsigned short *p);
void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, CylinderClsn *cc);
void _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(void *self, WithMeshClsn *wm, unsigned int j);
void func_ov032_02112044(char *c);
void _ZN9Animation7AdvanceEv(void *self);
void func_ov032_021113fc(void *self);
void _ZN12CylinderClsn5ClearEv(CylinderClsn *self);
void *_ZN5Actor13ClosestPlayerEv(void *self);
void _ZN12CylinderClsn6UpdateEv(CylinderClsn *self);
extern char data_ov032_02113aac[];
}

extern "C" int func_ov032_021121b4(char *c)
{
    if (_ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(c, (WithMeshClsn *)(c + 0x190), (ModelAnim *)(c + 0x34c), 3) != 0)
        return 1;

    DecIfAbove0_Short((unsigned short *)(c + 0x100));
    DecIfAbove0_Short((unsigned short *)(c + 0x42a));

    M *m = *(M **)(c + 0x3b0);
    if (m->pmf != 0)
        (((Klass *)c)->*(m->pmf))();

    *(short *)(c + 0x8c) = *(short *)(c + 0x92);
    *(short *)(c + 0x8e) = *(short *)(c + 0x94);
    *(short *)(c + 0x90) = *(short *)(c + 0x96);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, (CylinderClsn *)(c + 0x110));
    _ZN5Enemy12UpdateWMClsnER12WithMeshClsnj(c, (WithMeshClsn *)(c + 0x190), 0);
    func_ov032_02112044(c);

    if (*(void **)(c + 0x3b0) != (void *)data_ov032_02113aac) {
        *(int *)(c + 0x3a8) = 0x1000;
    } else {
        *(int *)(c + 0x3a8) = 0x2000;
    }

    _ZN9Animation7AdvanceEv(c + 0x39c);
    func_ov032_021113fc(c);
    _ZN12CylinderClsn5ClearEv((CylinderClsn *)(c + 0x110));
    _ZN12CylinderClsn5ClearEv((CylinderClsn *)(c + 0x150));

    void *p = _ZN5Actor13ClosestPlayerEv(c);
    if (p != 0 && *(unsigned char *)((char *)p + 0x6fb) == 0) {
        _ZN12CylinderClsn6UpdateEv((CylinderClsn *)(c + 0x110));
        _ZN12CylinderClsn6UpdateEv((CylinderClsn *)(c + 0x150));
    }

    return 1;
}
