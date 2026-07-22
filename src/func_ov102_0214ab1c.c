typedef unsigned short u16;
typedef unsigned char u8;

extern int _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(void *self, void *clsn);
extern int _ZN6Player16IsInsideOfCannonEv(void *player);
extern int _ZN6Player22IsBeingShotOutOfCannonEv(void *player);
extern int func_ov102_0214b248(u8 *c);
extern void func_ov102_0214b384(void *arg0, unsigned int arg1);
extern void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *m, void *f, int i, int fx, unsigned int j);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern int _ZN5Enemy27SpawnParticlesIfHitOtherObjER12CylinderClsn(void *self, void *clsn);
extern void _ZN12CylinderClsn5ClearEv(void *self);
extern void _ZN12CylinderClsn6UpdateEv(void *self);
extern void func_ov102_0214b53c(u8 *c);

extern int *data_ov102_0214e9c0;

int func_ov102_0214ab1c(u8 *self)
{
    int r4 = _ZN5Enemy14UpdateYoshiEatER12WithMeshClsn(self, self + 0x144);
    if (r4 == 0) {
        goto ret0;
    }

    *(unsigned int *)(((int)(self + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x10000000u;

    void *player = *(void **)(self + 0xd0);
    if (player != 0) {
        if (_ZN6Player16IsInsideOfCannonEv(player) != 0) {
            goto shocked;
        }
        if (_ZN6Player22IsBeingShotOutOfCannonEv(player) != 0) {
            goto shocked;
        }
        if (func_ov102_0214b248(self) == 0) {
            *(unsigned int *)(((int)(self) + 0xb0) & 0xFFFFFFFFFFFFFFFFLL) &= ~0xe0000u;
            self[0x107] = 0;
            return 1;
        }
        goto after_cannon;
    shocked:
        func_ov102_0214b384(self, 3);
    }
after_cannon:

    if (self[0x3f5] == 2) {
        self[0x3f5] = 3;
        *(int *)(self + 0x3dc) = 0;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x300, *(void **)((char *)&data_ov102_0214e9c0 + 4), 0, 0x1000, 0);
        *(u16 *)(self + 0x3e8) = 0x200;
        *(int *)(self + 0x9c) = -0x2000;
        *(unsigned int *)(((int)(self + 0x128)) & 0xFFFFFFFFFFFFFFFFLL) &= ~2u;
    }

    if (r4 == 2) {
        func_ov102_0214b384(self, 0x50);
    }

    if (r4 == 3) {
        if (_ZNK12WithMeshClsn10IsOnGroundEv(self + 0x144) != 0) {
            self[0x3f4] = 3;
        }
    }

    if (self[0x107] != 0) {
        if (*(u16 *)(self + 0x104) == 5) {
            *(unsigned int *)(((int)(self + 0x128)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x8000u;
        }
    }

    if (_ZN5Enemy27SpawnParticlesIfHitOtherObjER12CylinderClsn(self, self + 0x110) != 0) {
        goto hit;
    }
    if (r4 != 3 || _ZNK12WithMeshClsn10IsOnGroundEv(self + 0x144) == 0) {
        goto skip_hit;
    }

hit:
    func_ov102_0214b384(self, 4);
    self[0x107] = 0;
    return 1;

skip_hit:
    _ZN12CylinderClsn5ClearEv(self + 0x110);
    if (self[0x107] != 0) {
        if (*(u16 *)(self + 0x104) == 0) {
            _ZN12CylinderClsn6UpdateEv(self + 0x110);
        }
    }
    func_ov102_0214b53c(self);
    return 1;

ret0:
    return 0;
}
