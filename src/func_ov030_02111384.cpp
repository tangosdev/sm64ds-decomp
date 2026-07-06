//cpp
extern "C" int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(char *c, int a, int b);
extern "C" void _ZN5Actor9UpdatePosEP12CylinderClsn(char *c, char *clsn);
extern "C" int func_ov030_0211124c(char *c, char *mc);
extern "C" int func_ov030_02111384(char *c) {
    volatile int dummy[4];
    (void)&dummy;
    _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0);
    *(short*)(c+0x8e) = *(short*)(c+0x8e) + *(int*)(c+0x98);
    if (*(int*)(c+0x9c) != 0) {
        *(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) = *(int *)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFF) & ~1;
        _ZN5Actor9UpdatePosEP12CylinderClsn(c, (char*)0);
        func_ov030_0211124c(c, c+0x320);
        int a0 = *(int*)(c+0x60);
        int a1 = *(int*)(c+0x64);
        int m = *(int*)(c+0x4dc);
        int a2 = *(int*)(c+0x5c);
        *(int*)(m + 0x5c) = a2;
        *(int*)(m + 0x60) = a0 + 0x3c000;
        *(int*)(m + 0x64) = a1;
    }
    return 1;
}
