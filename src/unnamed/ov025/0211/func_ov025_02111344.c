extern void Matrix4x3_FromRotationX(void *, int);
void func_ov025_02111344(char *t)
{
    Matrix4x3_FromRotationX(t + 0xf0, *(short *)(t + 0x8c));
    *(int *)(t + 0x114) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x118) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x11c) = *(int *)(t + 0x64) >> 3;
}
