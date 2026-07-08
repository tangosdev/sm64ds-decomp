extern void func_0200d8c8(void* cam, void* v, int strength);
extern int CAMERA;
extern void _ZN5Actor15HugeLandingDustEb(void* thiz, int b);
extern void func_02012694(int a, void* b);

void func_ov066_02116ac4(char* c, int strength) {
    volatile int s0, s1, s2;
    func_0200d8c8((void*)CAMERA, c + 0x5c, strength);
    s0 = *(int*)(c + 0x5c);
    s1 = *(int*)(c + 0x60);
    s2 = *(int*)(c + 0x64);
    if (*(int*)(c + 0x49c) == 1)
        *(int*)(((long)c + 0x5c) & 0xFFFFFFFFFFFFFFFF) -= 0x80000;
    else
        *(int*)(((long)c + 0x5c) & 0xFFFFFFFFFFFFFFFF) += 0x80000;
    *(int*)(((long)c + 0x64) & 0xFFFFFFFFFFFFFFFF) += 0x80000;
    _ZN5Actor15HugeLandingDustEb(c, 1);
    func_02012694(0x143, c + 0x74);
    *(int*)(c + 0x5c) = s0;
    *(int*)(c + 0x60) = s1;
    *(int*)(c + 0x64) = s2;
}
