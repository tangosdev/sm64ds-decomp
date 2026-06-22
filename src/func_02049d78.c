typedef unsigned char u8;

extern int func_0204d958(void *p);
extern void func_0204d9a0(void *p, int r);
extern void func_0204d8e8(void *p, void *q);

void func_02049d78(void *a, void *b)
{
    u8 *ra = (u8 *)a;
    u8 *rb = (u8 *)b;
    int r;
    r = func_0204d958(rb + 8);
    while (r != 0) {
        func_0204d9a0(ra + 0x14, r);
        r = func_0204d958(rb + 8);
    }
    r = func_0204d958(rb + 0x10);
    while (r != 0) {
        func_0204d9a0(ra + 0x14, r);
        r = func_0204d958(rb + 0x10);
    }
    func_0204d8e8(ra + 4, b);
    func_0204d9a0(ra + 0xc, (int)b);
}
