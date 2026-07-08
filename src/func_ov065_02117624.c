extern int _ZN5Actor18HorzAngleToCPlayerEv(void* c);
extern void _Z14ApproachLinearRsss(short* p, short t, short step);
extern void Matrix4x3_FromRotationY(void* m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, short ang);
extern void MulVec3Mat4x3(void* dst, void* m, void* src);
extern int Vec3_Dist(void* a, void* b);
extern void func_ov065_02117944(void* c, void* p);
extern int MATRIX_SCRATCH_PAPER[];
extern int data_ov065_0211d6f0[];

int func_ov065_02117624(char* c) {
    int v[3];
    v[0] = 0; v[1] = 0; v[2] = 0;
    *(short*)(c + 0x434) = (short)_ZN5Actor18HorzAngleToCPlayerEv(c);
    _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x434), 0x500);
    _Z14ApproachLinearRsss((short*)(c + 0x92), 0x100, 0x500);
    v[2] = 0xa000;
    Matrix4x3_FromRotationY(MATRIX_SCRATCH_PAPER, *(short*)(c + 0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(MATRIX_SCRATCH_PAPER, *(short*)(c + 0x92));
    MulVec3Mat4x3(v, MATRIX_SCRATCH_PAPER, c + 0xa4);
    if (Vec3_Dist(c + 0x5c, c + 0x424) > 0x5dc000 || *(unsigned short*)(c + 0x100) == 0) {
        *(int*)(c + 0x430) = 0;
        func_ov065_02117944(c, data_ov065_0211d6f0);
    }
    return 1;
}
