extern void Matrix4x3_FromRotationY(void *, int);
void func_ov074_0212195c(char *t)
{
    Matrix4x3_FromRotationY(t + 0x22c, *(short *)(t + 0x8e));
    *(int *)(t + 0x250) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x254) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x258) = *(int *)(t + 0x64) >> 3;
}
