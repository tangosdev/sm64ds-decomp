extern void FUN_02029ab0(void);
extern int func_02008b08(void *cam);
extern short func_02008abc(void *cam);
extern short func_02008a70(void *cam);
extern int Sound_PlayIfNotActive(int a, int b, int c, int d);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short a);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short a);
extern void MulVec3Mat4x3(void *v, void *m, void *dst);
extern unsigned char data_020a0e40[];
extern short data_0209f4a0[];
extern unsigned char data_0209f4ae[];
extern int data_020a0e68[];

int func_020095e4(void *cam)
{
    int v[3];
    int off;
    int r7;
    short old186, old17e;
    char *c = (char *)cam;

    FUN_02029ab0();
    func_02008b08(cam);

    off = data_020a0e40[0] * 0x18;
    if (*(short *)((char *)data_0209f4a0 + off) != 0) {
        r7 = (data_0209f4ae[off] == 2) ? 0x240 : 0x200;
        old186 = *(short *)(c + 0x186);
        {
            short *p = (short *)(((int)c + 0x186));
            int a = func_02008abc(cam);
            *p -= (int)(((long long)r7 * a + 0x800) >> 12);
        }
        {
            int t = *(short *)(c + 0x186);
            if (t < -0x4000) t = -0x4000;
            else if (t > 0x4000) t = 0x4000;
            *(short *)(c + 0x186) = t;
        }
        *(short *)(c + 0x17c) = *(short *)(c + 0x19e) + *(short *)(c + 0x186);
        old17e = *(short *)(c + 0x17e);
        {
            short *p = (short *)(((int)c + 0x17e));
            *p += (int)(((long long)r7 * (long long)func_02008a70(cam) + 0x800) >> 12);
        }
        {
            int t = *(short *)(c + 0x17e);
            if (t < 0) t = 0;
            else if (t > 0x38e3) t = 0x38e3;
            *(short *)(c + 0x17e) = t;
        }
        if (*(short *)(c + 0x186) != old186 || *(short *)(c + 0x17e) != old17e) {
            *(int *)(c + 0x158) = Sound_PlayIfNotActive(*(int *)(c + 0x158), 3, 0x181, 0);
        }
    }

    v[0] = 0;
    v[1] = 0;
    v[2] = -0x280000;
    Matrix4x3_FromTranslation(data_020a0e68, *(int *)(c + 0x8c), *(int *)(c + 0x90), *(int *)(c + 0x94));
    Matrix4x3_ApplyInPlaceToRotationY(data_020a0e68, *(short *)(c + 0x17c));
    Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, *(short *)(c + 0x17e));
    MulVec3Mat4x3(v, data_020a0e68, c + 0x80);
    *(short *)(*(int *)(c + 0x110) + 0x8e) = *(short *)(c + 0x17c) - 0x8000;
    return 1;
}
