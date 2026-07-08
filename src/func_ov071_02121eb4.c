typedef int Fix12;
extern int _ZN11ShadowModel12InitCylinderEv(void *self);
extern int _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(void *self, void *actor, void *v, Fix12 r, int t1, unsigned int u1, unsigned int u2);
extern int _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(void *self, void *actor, Fix12 a, int b, void *v, int c);
extern void func_ov071_02121c6c(char *c);
extern void *data_ov071_021230b8;
struct M48 { int w[12]; };
extern struct M48 _ZN9Matrix3x38IDENTITYE;
int func_ov071_02121eb4(char *c) {
    if (!_ZN11ShadowModel12InitCylinderEv(c + 0xd4)) return 0;
    _ZN25MovingCylinderClsnWithPos4InitEP5ActorRK7Vector35Fix12IiES6_jj(c + 0xfc, c, &data_ov071_021230b8, 0x23000, 0x3c000, 0x200002, 0x8000);
    *(int*)(c+0x80) = 0x1000;
    *(int*)(c+0x84) = 0x1000;
    *(int*)(c+0x88) = 0x1000;
    _ZN12WithMeshClsn4InitEP5Actor5Fix12IiES3_P10Vector3_16S5_(c + 0x13c, c, 0x19000, 0x19000, 0, 0);
    *(int*)(c+0x9c) = 0;
    *(int*)(c+0xa0) = 0;
    *(int*)(c+0x98) = 0xa000;
    *(short*)(c+0x330) = 0x96;
    *(struct M48*)(c+0x2f8) = _ZN9Matrix3x38IDENTITYE;
    func_ov071_02121c6c(c);
    return 1;
}
