extern void Matrix4x3_FromRotationY(void *, int);
void func_ov034_021129ec(char *t)
{
    int i;
    char *r8 = t;
    char *r6 = t + 0x110;
    char *r5 = t;
    char *r4 = t;
    for (i = 0; i < 5; i++) {
        Matrix4x3_FromRotationY(r6 + 0x1c, *(short *)(r8 + 0x446));
        *(int *)(r4 + 0x150) = *(int *)(r5 + 0x3cc) >> 3;
        *(int *)(r4 + 0x154) = *(int *)(r5 + 0x3d0) >> 3;
        *(int *)(r4 + 0x158) = *(int *)(r5 + 0x3d4) >> 3;
        r8 += 6;
        r6 += 0x64;
        r5 += 0xc;
        r4 += 0x64;
    }
}
