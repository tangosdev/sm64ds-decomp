struct M48 { int w[12]; };

extern void _Z14ApproachLinearRiii(int* v, int target, int step);
extern void ApproachAngle(short* v, int target, int a, int b, int c);
extern void Matrix4x3_ApplyInPlaceToTranslation(struct M48* m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationX(struct M48* m, int a);
extern void Matrix4x3_ApplyInPlaceToRotationY(struct M48* m, int a);
extern void Matrix4x3_ApplyInPlaceToRotationZ(struct M48* m, int a);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(struct M48* m, int x, int y, int z);
extern unsigned int _ZNK6Player14GetBodyModelIDEjb(void* self, unsigned int j, int b);
extern unsigned int func_ov002_020becf4(void* self, unsigned int j, int b);

extern struct M48 data_020a0e68;

int func_ov002_020e3f90(char* self) {
    struct S { short angs[3]; volatile int v[3]; int tgt[3]; } s;
    int b;
    int t;
    int t1;
    int t2;
    char* p;
    unsigned int n;
    char* m;

    if (*(char**)(self + 0x35c) == 0) return 0;
    if (*(char**)(*(char**)(self + 0x35c) + 0xc8) == 0) return 0;

    b = 0;
    s.v[0] = 0;
    s.v[1] = 0;
    s.v[2] = 0;
    data_020a0e68 = *(struct M48*)(*(char* volatile *)(*(char* volatile *)(self + 0x35c) + 0xc8));
    p = *(char**)(self + 0x35c);
    if (*(unsigned short*)(p + 0xc) == 0xbf) b = 1;
    if (b != 0) {
        t = (unsigned int)(*(int*)(p + 0x358) != 0);
        if (t == 0) return 0;
        t = (unsigned int)((*(int*)(self + 0xb0) & 0x4000) != 0);
        if (t == 0) return 0;
        if ((unsigned int)((*(int*)(p + 0x63c) >> 2) - 0x6c) <= 1) {
            s.tgt[0] = -0x3000;
            s.tgt[1] = -0x1000;
            s.tgt[2] = -0x4000;
            s.angs[0] = -0x2000;
            s.angs[1] = -0x1000;
            s.angs[2] = -0x1000;
        } else {
            s.tgt[0] = 0;
            s.tgt[1] = -0x3000;
            s.tgt[2] = -0x4000;
            s.angs[0] = 0x1000;
            s.angs[1] = 0x7000;
            s.angs[2] = 0;
        }
        _Z14ApproachLinearRiii((int*)(self + 0x744), s.tgt[0], 0x400);
        _Z14ApproachLinearRiii((int*)(self + 0x748), s.tgt[1], 0x400);
        _Z14ApproachLinearRiii((int*)(self + 0x74c), s.tgt[2], 0x400);
        ApproachAngle((short*)(self + 0x8c), s.angs[0], 0x10, 0x1000, 0x100);
        ApproachAngle((short*)(self + 0x8e), s.angs[1], 0x10, 0x1000, 0x100);
        ApproachAngle((short*)(self + 0x90), s.angs[2], 0x10, 0x1000, 0x100);
        Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, *(int*)(self + 0x744), *(int*)(self + 0x748), *(int*)(self + 0x74c));
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, *(short*)(self + 0x8e));
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, *(short*)(self + 0x8c));
        Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, *(short*)(self + 0x90));
    } else {
        t = (unsigned int)(*(unsigned short*)(p + 0xc) == 0xbd);
        if (t != 0) {
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0x5800, -0x6800, -0x2000);
            Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, 0x3c71, 0x38e, -0x4e38);
        } else {
            Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0x5800, 0x5000, 0xb000);
            Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, -0x600, -0x2600, 0);
        }
    }
    *(struct M48*)(self + 0x5bc) = data_020a0e68;
    t = ((volatile struct M48*)&data_020a0e68)->w[9];
    t1 = ((volatile struct M48*)&data_020a0e68)->w[10];
    t2 = ((volatile struct M48*)&data_020a0e68)->w[11];
    s.v[1] = t1;
    s.v[2] = t2;
    s.v[0] = t;
    *(int*)(self + 0x5c) = t << 3;
    *(int*)(self + 0x60) = s.v[1] << 3;
    *(int*)(self + 0x64) = s.v[2] << 3;
    *(short*)(self + 0x94) = *(short*)(*(char**)(self + 0x35c) + 0x8e);
    n = _ZNK6Player14GetBodyModelIDEjb(self, *(int*)(self + 8) & 0xff, 0);
    m = *(char**)((self + (n << 2)) + 0xdc);
    *(struct M48*)(m + 0x1c) = data_020a0e68;
    *(struct M48*)(self + 0x190) = data_020a0e68;
    n = func_ov002_020becf4(self, *(int*)(self + 8) & 0xff, 0);
    if (n != 9 && n != 8) {
        m = *(char**)((self + (n << 2)) + 0x154);
        if (m != 0) {
            *(struct M48*)(m + 0x1c) = data_020a0e68;
        }
    }
    return 1;
}
