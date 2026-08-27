extern void Matrix4x3_FromRotationY(void *, int);
void func_ov029_02112710(char *t)
{
    Matrix4x3_FromRotationY(t + 0x33c, *(short *)(t + 0x8e));
    *(int *)(t + 0x360) = *(int *)(t + 0x5c) >> 3;
    *(int *)(t + 0x364) = *(int *)(t + 0x60) >> 3;
    *(int *)(t + 0x368) = *(int *)(t + 0x64) >> 3;
}
