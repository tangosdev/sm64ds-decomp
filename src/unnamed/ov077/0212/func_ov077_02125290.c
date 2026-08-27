extern void Matrix4x3_FromRotationZXYExt(void *, int, int, int);
void func_ov077_02125290(char *t)
{
    int v = *(int *)(t + 0x3d8);
    int b = 1;
    char *m;
    if (v != 0) b = (v == 4);
    if (b) m = t + 0xf0;
    else m = t + 0x140;
    Matrix4x3_FromRotationZXYExt(m, *(short *)(t + 0x8c), *(short *)(t + 0x8e), *(short *)(t + 0x90));
    *(int *)(m + 0x24) = *(int *)(t + 0x5c) >> 3;
    *(int *)(m + 0x28) = *(int *)(t + 0x60) >> 3;
    *(int *)(m + 0x2c) = *(int *)(t + 0x64) >> 3;
}
