extern void Matrix4x3_FromRotationY(void *, int);
void func_ov065_02118cc4(char *t)
{
    Matrix4x3_FromRotationY(t + 0x108, *(short *)(t + 0x8e));
    *(int *)(t + 0x12c) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x130) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x134) = *(int *)(t + 0x64) >> 3;
}
