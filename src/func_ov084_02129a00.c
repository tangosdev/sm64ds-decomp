typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

typedef struct Vector3 { int x, y, z; } Vector3;

extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void *self, void *clsn);
extern int _ZN8CapEnemy16GetCapEatenOffItERK7Vector3(void *self, Vector3 *v);
extern void func_ov084_02129168(char *c, char *actor);
extern void _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File(void *bmd, void *bma);
extern void _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(void *m, void *f, int a, int fix, unsigned int j);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern int _ZN5Enemy27SpawnParticlesIfHitOtherObjER12CylinderClsn(void *self, void *clsn);
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_020aea30(void *self, void *actor, void *clsn, int a);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, void *f, int a, int fix, unsigned int j);
extern void func_ov084_0212a580(void *self);
extern void func_ov084_021294d0(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void _ZN9Animation7AdvanceEv(void *self);
extern int _ZNK12WithMeshClsn13JustHitGroundEv(void *self);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);

extern int *data_ov084_02130cf8;
extern int *data_ov084_0213088c;
extern int *data_ov084_02130ce0;

int func_ov084_02129a00(char *self) {
    int r4 = _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(self, self + 0x1b4);
    if (r4 == 0)
        goto ret0;
    if (r4 == 1) {
        Vector3 v;
        char *actor = *(char **)(self + 0xd0);
        v.x = 0;
        v.y = 0x6c000;
        v.z = 0;
        if (_ZN8CapEnemy16GetCapEatenOffItERK7Vector3(self, &v) != 0) {
            func_ov084_02129168(self, actor);
            *(int *)(self + 0x98) = -0xf000;
            *(int *)(self + 0xa8) = 0x14000;
            _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File((void *)((int *)&data_ov084_02130cf8)[1], &data_ov084_0213088c);
            _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(self + 0x3fc, &data_ov084_0213088c, 0x40000000, 0x1000, 0);
            *(int *)(self + 0x404) = 0;
            _ZN12CylinderClsn5ClearEv(self + 0x180);
            return 0;
        }
    } else if (r4 == 3) {
        if (_ZNK12WithMeshClsn10IsOnGroundEv(self + 0x1b4))
            *(int *)(((int)self + 0x98) & 0xFFFFFFFFFFFFFFFF) >>= 1;
    }

    if (_ZN5Enemy27SpawnParticlesIfHitOtherObjER12CylinderClsn(self, self + 0x180) != 0) {
        void *actor = _ZN5Actor10FindWithIDEj(*(int *)(self + 0x1a4));
        *(int *)(self + 0x10c) = 7;
        func_020aea30(self, actor, self + 0x1b4, 7);
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, (void *)((int *)&data_ov084_02130ce0)[1], 0x40000000, 0x1000, 0);
        *(int *)(((int)self + 0x198) & 0xFFFFFFFFFFFFFFFF) |= 1;
        return 1;
    }

    func_ov084_0212a580(self);
    _ZN12CylinderClsn5ClearEv(self + 0x180);
    if (*(u8 *)(self + 0x107) != 0) {
        func_ov084_021294d0(self);
        {
            u16 s = *(u16 *)(self + 0x104);
            if (s == 0) {
                _ZN12CylinderClsn6UpdateEv(self + 0x180);
            } else if (s == 5) {
                _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x370, (void *)((int *)&data_ov084_02130ce0)[1], 0x40000000, 0x1000, 0);
                *(s16 *)(((int)self + 0x94) & 0xFFFFFFFFFFFFFFFF) += 0x8000;
                *(int *)(self + 0x98) = -*(int *)(self + 0x98);
                _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File((void *)((int *)&data_ov084_02130cf8)[1], &data_ov084_0213088c);
                _ZN15MaterialChanger7SetFileER8BMA_Filei5Fix12IiEj(self + 0x3fc, &data_ov084_0213088c, 0x40000000, 0x1000, 0);
                *(int *)(self + 0x404) = 0;
            }
        }
        _ZN9Animation7AdvanceEv(self + 0x3c0);
        if (_ZNK12WithMeshClsn13JustHitGroundEv(self + 0x1b4))
            func_ov084_02129168(self, 0);
    }

    if (*(int *)(self + 0x460) != 3)
        goto ret1;
    if (r4 < 3)
        goto ret1;
    if (*(int *)(self + 0x60) >= *(int *)(self + 0x420) - 0x3e8000)
        goto ret1;
    _ZN9ActorBase18MarkForDestructionEv(self);
    return 1;
ret1:
    return 1;
ret0:
    return 0;
}
