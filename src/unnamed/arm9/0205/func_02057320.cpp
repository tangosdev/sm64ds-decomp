//cpp
extern "C" {
extern int data_020a6124;
extern void func_020572b8(int a, void *b, int c);
extern void func_020570a8(void);
extern void MultiStore_Int(int v, void *dst, int n);
extern void func_02057218(int a, void *b, int c);

void func_02057320(void) {
    volatile int li;
    char *base = (char*)0x27ffff0;
    if (data_020a6124 != 0) return;
    data_020a6124 = 1;
    *(int*)base = 0;
    func_020572b8(0x7e, base, 0);
    while (*(unsigned short*)(base + 6) != 0) {
        func_020570a8();
    }
    *(int*)0x27fffb0 = -1;
    *(int*)0x27fffb4 = -0x10000;
    li = 0;
    MultiStore_Int(li, (void*)0x27fffc0, 0x28);
    *(volatile unsigned short*)0x4000204 |= 0x800;
    *(volatile unsigned short*)0x4000204 |= 0x80;
    func_02057218(0x7e, base, 0);
    func_020572b8(0x7f, base, 0);
}
}
