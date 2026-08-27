extern void Matrix4x3_FromRotationY(void *, int);
void func_ov062_0211afbc(char *t)
{
    Matrix4x3_FromRotationY(t + 0x124, *(short *)(t + 0x8e));
    *(int *)(t + 0x148) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x14c) = (*(int *)(t + 0x60) + 0x3c000) >> 3;
    *(int *)(t + 0x150) = *(int *)(t + 0x64) >> 3;
}
