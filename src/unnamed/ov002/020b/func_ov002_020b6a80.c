extern void Matrix4x3_FromRotationZXYExt(void* dst, int a, int b, int c2);
int func_ov002_020b6a80(char* c) {
    Matrix4x3_FromRotationZXYExt(c+0xf0, 0, *(short*)(c+0x8e), *(short*)(c+0x90));
    *(int*)(c+0x114) = *(int*)(c+0x5c) >> 3;
    *(int*)(c+0x118) = *(int*)(c+0x60) >> 3;
    int z = *(int*)(c+0x64) >> 3;
    *(int*)(c+0x11c) = z;
    return z;
}
