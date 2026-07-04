//cpp
extern "C" {
extern int func_02061428(int count, ...);
extern void _ZN4CP1514FlushDataCacheEjj(unsigned int a, unsigned int b);
extern void func_0206165c(int i, int val);
extern char *func_02061548(void);
extern int func_02061558(short a0, unsigned short count);

int func_02061ee4(int a, char *b) {
    int rc;
    char *d;
    unsigned short h;
    rc = func_02061428(3, 2, 3, 5);
    if (rc != 0) return rc;
    if (b == 0) return 6;
    if (*(int*)b == 0) return 6;
    h = *(unsigned short*)(b + 4);
    if (h < 1 || h > 0xe) return 6;
    _ZN4CP1514FlushDataCacheEjj((unsigned int)b, 0x20);
    func_0206165c(0xa, a);
    d = *(char**)(func_02061548() + 0xc);
    *(short*)(d) = 0xa;
    *(short*)(d + 2) = *(unsigned short*)(b + 4);
    *(int*)(d + 4) = *(int*)b;
    *(short*)(d + 8) = *(unsigned short*)(b + 6);
    *(short*)(d + 0xa) = *(unsigned short*)(b + 8);
    *(short*)(d + 0xc) = *(unsigned short*)(b + 0xa);
    *(short*)(d + 0xe) = *(unsigned short*)(b + 0xc);
    rc = func_02061558(0xa, 0);
    if (rc == 0) return 2;
    return rc;
}
}
