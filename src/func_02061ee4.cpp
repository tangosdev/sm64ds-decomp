//cpp
extern "C" {
extern int WM_CheckStateEx(int count, ...);
extern void _ZN4CP1514FlushDataCacheEjj(unsigned int a, unsigned int b);
extern void WM_SetCallbackTable(int i, int val);
extern char *func_02061548(void);
extern int WM_SendCommand(short a0, unsigned short count);

int func_02061ee4(int a, char *b) {
    int rc;
    char *d;
    unsigned short h;
    rc = WM_CheckStateEx(3, 2, 3, 5);
    if (rc != 0) return rc;
    if (b == 0) return 6;
    if (*(int*)b == 0) return 6;
    h = *(unsigned short*)(b + 4);
    if (h < 1 || h > 0xe) return 6;
    _ZN4CP1514FlushDataCacheEjj((unsigned int)b, 0x20);
    WM_SetCallbackTable(0xa, a);
    d = *(char**)(func_02061548() + 0xc);
    *(short*)(d) = 0xa;
    *(short*)(d + 2) = *(unsigned short*)(b + 4);
    *(int*)(d + 4) = *(int*)b;
    *(short*)(d + 8) = *(unsigned short*)(b + 6);
    *(short*)(d + 0xa) = *(unsigned short*)(b + 8);
    *(short*)(d + 0xc) = *(unsigned short*)(b + 0xa);
    *(short*)(d + 0xe) = *(unsigned short*)(b + 0xc);
    rc = WM_SendCommand(0xa, 0);
    if (rc == 0) return 2;
    return rc;
}
}
