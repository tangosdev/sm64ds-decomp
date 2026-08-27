//cpp
extern "C" {
extern void Matrix4x3_FromRotationY(void*, short);
extern void MulVec3Mat4x3(void*, void*, void*);
extern void Vec3_Add(void*, void*, void*);
extern char data_020a0e68[];
extern char data_ov091_02134cdc[];
void func_ov091_02130fac(int c) {
    unsigned char *b = (unsigned char*)c;
    char tmp[0x18];
    *(char*)(b + 0x3a0) = 0;
    *(short*)(b + 0x8c) = *(short*)(b + 0x380);
    *(short*)(b + 0x8e) = *(short*)(b + 0x382);
    *(short*)(b + 0x90) = *(short*)(b + 0x384);
    *(int*)(b + 0x5c) = *(int*)(b + 0x388);
    *(int*)(b + 0x60) = *(int*)(b + 0x38c);
    *(int*)(b + 0x64) = *(int*)(b + 0x390);
    *(char*)(b + 0x394) = (char)(*(int*)(b + 8) & 0xf);
    Matrix4x3_FromRotationY((void*)data_020a0e68, *(short*)(b + 0x8e));
    MulVec3Mat4x3(
        (void*)(data_ov091_02134cdc + *(unsigned char*)(b + 0x395) * 0x78 + *(unsigned char*)(b + 0x394) * 0xc),
        (void*)data_020a0e68,
        (void*)tmp);
    Vec3_Add((void*)(tmp + 0xc), (void*)(b + 0x388), (void*)tmp);
    *(int*)(b + 0x5c) = *(int*)(tmp + 0xc);
    *(int*)(b + 0x60) = *(int*)(tmp + 0x10);
    *(int*)(b + 0x64) = *(int*)(tmp + 0x14);
}
}
