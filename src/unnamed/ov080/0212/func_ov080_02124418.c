extern void Matrix4x3_FromRotationY(void *, int);
void func_ov080_02124418(char *t)
{
    Matrix4x3_FromRotationY(t + 0x12c, *(short *)(t + 0x8e));
    *(int *)(t + 0x150) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x154) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x158) = *(int *)(t + 0x64) >> 3;
}
