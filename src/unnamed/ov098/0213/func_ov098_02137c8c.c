extern void Matrix4x3_FromRotationY(void *, int);
void func_ov098_02137c8c(char *t)
{
    Matrix4x3_FromRotationY(t + 0x348, *(short *)(t + 0x8e));
    *(int *)(t + 0x36c) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x370) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x374) = *(int *)(t + 0x64) >> 3;
}
