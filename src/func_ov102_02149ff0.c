struct Mtx43 { int m[12]; };
extern void Matrix4x3_FromRotationY(void *m, int angle);
extern int data_0209caa0[];

void func_ov102_02149ff0(char *c)
{
    Matrix4x3_FromRotationY(c + 0xf0, *(short *)(c + 0x8e));
    *(int *)(c + 0x114) = *(int *)(c + 0x5c) >> 3;
    *(int *)(c + 0x118) = (*(int *)(c + 0x60) + *(int *)(c + 0x3dc)) >> 3;
    *(int *)(c + 0x11c) = *(int *)(c + 0x64) >> 3;
    if (data_0209caa0[1] & 0x80000000)
        return;
    int b = (int)(*(unsigned short *)(c + 0xc) == 0x14);
    if (b == 0)
        return;
    *(struct Mtx43 *)(c + 0x33c) = *(struct Mtx43 *)(c + 0xf0);
}
