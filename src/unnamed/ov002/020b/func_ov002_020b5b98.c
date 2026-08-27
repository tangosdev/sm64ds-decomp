extern void Matrix4x3_FromRotationZXYExt(void *, int, int, int);
void func_ov002_020b5b98(char *t)
{
    Matrix4x3_FromRotationZXYExt(t + 0xf0, *(short *)(t + 0x8c), *(short *)(t + 0x8e), *(short *)(t + 0x90));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}
