extern int func_0203cfdc(void *a, void *b);
extern void func_0200f97c(void *c, int a);
extern void func_02015024(void *p);

int func_ov030_02112ff8(char *c)
{
    *(int *)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x80000;
    if (func_0203cfdc(c + 0x380, c + 0x5c) < 0x514000 &&
        *(int *)(c + 0x60) > *(int *)(c + 0x384) - 0x12c000) {
        *(unsigned char *)(c + 0x3c7) = 0;
        func_0200f97c(c, 1);
    } else {
        *(unsigned char *)(c + 0x3c7) = 3;
    }
    *(int *)(c + 0x98) = 0;
    *(unsigned char *)(c + 0x3c6) = 0x3c;
    func_02015024(c + 0x160);
    *(int *)(c + 0x3b8) = *(int *)(c + 0x3b4);
    *(int *)(c + 0x3b4) = 6;
    return 1;
}
