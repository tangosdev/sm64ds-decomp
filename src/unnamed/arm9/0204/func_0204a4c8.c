extern int func_0204d9a0(char *dst, char *src);

char *func_0204a4c8(char *(*alloc)(int), int a, int b, int c, unsigned short arg5) {
    int i;
    char *p;
    char *q;
    char *r;
    p = alloc(0x3c);
    *(short*)(p + 0x28) = (short)a;
    *(short*)(p + 0x2a) = (short)b;
    *(short*)(p + 0x2c) = (short)c;
    *(short*)(p + 0x2e) = (short)arg5;
    *(char**)(p) = (char*)alloc;
    *(int*)(p + 8) = 0;
    *(int*)(p + 0x10) = 0;
    *(int*)(p + 0x18) = 0;
    *(int*)(p + 4) = 0;
    *(int*)(p + 0xc) = 0;
    *(int*)(p + 0x14) = 0;
    *(int*)(p + 0x30) = 0;
    q = alloc(a * 0x78);
    for (i = 0; i < a; i++) {
        func_0204d9a0(p + 0xc, q);
        q += 0x78;
    }
    r = alloc(b * 0x44);
    for (i = 0; i < b; i++) {
        func_0204d9a0(p + 0x14, r);
        r += 0x44;
    }
    *(int*)(p + 0x1c) = 0;
    *(int*)(p + 0x20) = 0;
    *(unsigned short*)(p + 0x26) = 0;
    *(short*)(p + 0x24) = *(unsigned short*)(p + 0x26);
    return p;
}
