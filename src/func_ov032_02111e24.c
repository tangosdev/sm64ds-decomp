extern void _Z14ApproachLinearRiii(int* p, int t, int s);
extern int func_ov032_02111350(char* c);
extern int Vec3_HorzAngle(void* a, void* b);
extern void _Z14ApproachLinearRsss(short* p, int t, int s);
extern int Vec3_VertAngle(void* a, void* b);
extern void Matrix4x3_FromRotationY(void* m, short ang);
extern void Matrix4x3_ApplyInPlaceToRotationX(void* m, short ang);
extern void MulVec3Mat4x3(void* in, void* m, void* out);
extern int func_ov032_02111254(void* c);
extern int func_ov032_02111ff4(void* c, void* p);
extern int RandomIntInternal(int* seed);
extern int data_020a0e68;
extern char data_ov032_02113aac[];
extern char data_ov032_02113a9c[];
extern char data_ov032_02113a8c[];
extern int data_0209e650[];

int func_ov032_02111e24(char* c) {
    int in[3];
    int out[3];
    int ang;

    _Z14ApproachLinearRiii((int*)(c+0x98), 0x5000, 0x333);
    if (func_ov032_02111350(c) == 1) {
        *(short*)(c+0x100) = 0x28;
        *(short*)(c+0x400+0x2a) = 0x28;
        ang = Vec3_HorzAngle(c+0x5c, c+0x40c);
        *(short*)(c+0x400+0x30) = ang;
    }
    _Z14ApproachLinearRsss((short*)(c+0x94), *(short*)(c+0x400+0x30), 0x100);
    ang = Vec3_VertAngle(c+0x5c, c+0x40c);
    _Z14ApproachLinearRsss((short*)(c+0x92), ang, 0x100);
    in[2] = 0;
    in[2] = 0x5000;
    in[0] = 0;
    in[1] = 0;
    out[0] = 0;
    out[1] = 0;
    out[2] = 0;
    Matrix4x3_FromRotationY(&data_020a0e68, *(short*)(c+0x8e));
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(c+0x8c));
    MulVec3Mat4x3(in, &data_020a0e68, out);
    *(int*)(c+0xa8) = out[1];
    if (func_ov032_02111254(c) == 1) {
        func_ov032_02111ff4(c, data_ov032_02113aac);
        return 1;
    }
    if (*(unsigned short*)(c+0x100) == 0) {
        unsigned int r = (unsigned int)RandomIntInternal(data_0209e650);
        if (((r >> 8) & 3) == 0) {
            func_ov032_02111ff4(c, data_ov032_02113a9c);
        } else {
            func_ov032_02111ff4(c, data_ov032_02113a8c);
        }
    }
    return 1;
}
