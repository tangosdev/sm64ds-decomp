extern void Matrix4x3_FromRotationY(void *, int);
void func_ov077_02126dac(char *t)
{
    Matrix4x3_FromRotationY(t + 0x35c, *(short *)(t + 0x8e));
    *(int *)(t + 0x380) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x384) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x388) = *(int *)(t + 0x64) >> 3;
}
