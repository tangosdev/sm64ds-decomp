extern void Matrix4x3_FromRotationY(void *, int);

void func_ov029_021126dc(char *c) {
    Matrix4x3_FromRotationY(c + 0x370, *(short *)(c + 0x8e));
    *(int *)(c + 0x394) = *(int *)(c + 0x5c);
    *(int *)(c + 0x398) = *(int *)(c + 0x60);
    *(int *)(c + 0x39c) = *(int *)(c + 0x64);
}
