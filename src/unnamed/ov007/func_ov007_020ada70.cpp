//cpp
extern "C" {
extern void func_ov007_020c32e8(void *a, int b, int c, int d);
extern void _ZN4CP1527FlushAndInvalidateDataCacheEjj(unsigned int a, unsigned int b);
extern void _ZN3GXS11LoadOBJPlttEPKvjj(const void *p, unsigned int a, unsigned int b);
extern void RequestScreenFade(int a, int b, int c);
extern void *data_ov007_0210342c;
extern void func_ov007_020adb84(void);

void func_ov007_020ada70(void) {
    char *g = (char*)*(void**)&data_ov007_0210342c;
    short st = *(short*)(*(int*)(g + 0x40));
    char *r5 = (char*)*(int*)(g + 0x74);
    int n = 0xc;
    switch (st) {
        case 4:
            func_ov007_020c32e8(*(void**)(*(int*)(r5 + 0x2c) + 8), 3, 0, 0);
            break;
        case 5:
            func_ov007_020c32e8(*(void**)(*(int*)(r5 + 0x2c) + 0xc), 3, 0, 0);
            n = 0xd;
            break;
        case 6:
            func_ov007_020c32e8(*(void**)(*(int*)(r5 + 0x2c) + 0x10), 3, 0, 0);
            n = 0xe;
            break;
        case 7:
            func_ov007_020c32e8(*(void**)(*(int*)(r5 + 0x2c) + 0x14), 3, 0, 0);
            n = 0xf;
            break;
    }
    int base = *(int*)(*(int*)(*(int*)(r5 + 0x2c) + 4));
    char *dst = (char*)(base + (n << 5));
    _ZN4CP1527FlushAndInvalidateDataCacheEjj((unsigned int)dst, 0x20);
    _ZN3GXS11LoadOBJPlttEPKvjj(dst, 0x180, 0x20);
    RequestScreenFade(9, 2, (int)&func_ov007_020adb84);
}
}
