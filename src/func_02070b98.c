typedef unsigned char u8;
typedef short s16;
void func_02070c68(void* x);
void func_020715e0(void* thiz, int val);

#pragma opt_strength_reduction off
void func_02070b98(void* a0, int a1, int a2, void* d)
{
    int n;
    char* c = (char*)d;
    int i;
    n = *(s16*)((char*)a0 + 2);
    func_02070c68(c);
    if (*(u8*)(c + 5) > 9) return;
    if (n > 0x20) n = 0x20;
    func_020715e0(c, n);
    while (*(u8*)(c + 4) < n) {
        int t = *(u8*)(c + 4);
        *(u8*)(c + 4) += 1;
        (c + t)[5] = 0;
    }
    *(s16*)(((int)c + 2) & 0xFFFFFFFFFFFFFFFF) -= *(u8*)(c + 4) - 1;
    for (i = 0; i < *(u8*)(c + 4); i++) {
        *(u8*)(((int)c + i + 5) & 0xFFFFFFFFFFFFFFFF) += 0x30;
    }
}
