extern void Matrix4x3_FromRotationY(void *, int);
void func_ov002_020baf80(char *t)
{
    Matrix4x3_FromRotationY(t + 0x380, *(short *)(t + 0x8e));
    *(int *)(t + 0x3a4) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x3a8) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x3ac) = *(int *)(t + 0x64) >> 3;
}
