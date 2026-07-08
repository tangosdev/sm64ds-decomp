typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;

extern void _ZN5Actor9UpdatePosEP12CylinderClsn(void *self, void *clsn);
extern int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(u32 id, u32 a, int fix, int t1, int t2, void *vec);
extern void func_ov060_0211712c(void *self);
extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern int RandomIntInternal(int *seed);
extern void func_ov060_02116518(char *self, u32 kind, int a2, int a3);
extern void func_ov060_021172e0(void *self);
extern void _ZN9ActorBase18MarkForDestructionEv(void *self);

extern int RNG_STATE;

void func_ov060_02116f90(char *self) {
    _ZN5Actor9UpdatePosEP12CylinderClsn(self, 0);
    if (*(int *)(self + 0xa8) < -0x4000)
        *(int *)(self + 0xa8) = -0x4000;

    if (*(int *)(self + 0x36c) == 0) {
        *(int *)(self + 0x37c) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
            *(int *)(self + 0x37c), 0x9e, *(int *)(self + 0x5c), *(int *)(self + 0x60) + 0x37000, *(int *)(self + 0x64), 0);
        *(int *)(((int)self + 0x2e8) & 0xFFFFFFFFFFFFFFFF) |= 1;
        func_ov060_0211712c(self);
        if (_ZNK12WithMeshClsn10IsOnGroundEv(self + 0x110)) {
            (*(int *)(((int)self + 0x36c) & 0xFFFFFFFFFFFFFFFF))++;
            if (*(int *)(self + 0x35c) == 7) {
                *(int *)(self + 0x360) = 0x6000;
            } else {
                *(int *)(self + 0x360) = ((u32)RandomIntInternal(&RNG_STATE) >> 16 & 0xfff) * 2 + 0x6000;
            }
            *(int *)(self + 0x98) = 0;
            *(int *)(self + 0xa8) = 0;
            *(int *)(self + 0x9c) = 0;
            *(int *)(self + 0x380) = 0;
            *(int *)(self + 0x37c) = *(int *)(self + 0x380);
        }
    } else {
        func_ov060_02116518(self, 0x9c, 1, *(int *)(self + 0x360) * 0xc);
        *(int *)(((int)self + 0x2e8) & 0xFFFFFFFFFFFFFFFF) &= ~1;
        if (*(u16 *)(self + 0x374) > *(int *)(self + 0x360) * 0xa / 0x1000 + 5) {
            *(int *)(((int)self + 0x360) & 0xFFFFFFFFFFFFFFFF) -= 0x266;
            if (*(int *)(self + 0x360) <= 0)
                func_ov060_021172e0(self);
        }
    }
    if (*(int *)(self + 0x60) < 0)
        _ZN9ActorBase18MarkForDestructionEv(self);
}
