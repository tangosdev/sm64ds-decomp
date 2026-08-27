extern char* func_ov007_020c3df4(int a, int b);
extern void func_ov007_020c15ec(char* p);
extern void func_ov007_020c16dc(void* a, int b, int c, int d, void* e, void* f);

void* func_ov007_020c162c(void* a0, int a1, int a2, int a3, int a4, unsigned short a5)
{
    char* r6 = func_ov007_020c3df4(0, 0x2c);
    int r5 = 8 << a2;
    int r4 = 8 << a3;
    func_ov007_020c15ec(r6);
    *(void**)(r6 + 4) = a0;
    *(int*)(r6 + 8) = a1;
    *(int*)(r6 + 0x14) = a2;
    *(int*)(r6 + 0x18) = a3;
    *(int*)(r6 + 0x1c) = a4;
    *(short*)(r6 + 0x28) = a5;
    if (a1 != 0) *(int*)r6 |= 1;
    if (a4 != 0) *(int*)r6 |= 2;
    func_ov007_020c16dc(a0, r5, r4, a5, r6 + 0xc, r6 + 0x20);
    return r6;
}
