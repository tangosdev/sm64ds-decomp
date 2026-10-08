extern void func_ov007_020c3054(int* arr, int n, int* pmin, int* pmax);

void func_ov007_020c26fc(char* c)
{
    int mn0 = 0, mn1 = 0, mx0 = 0, mx1 = 0;
    *(short*)(c + 0x10) = 0;
    *(short*)(c + 0xe) = *(unsigned short*)(c + 0x10);
    *(short*)(c + 0xc) = *(unsigned short*)(c + 0xe);
    *(short*)(c + 0x16) = 0;
    *(short*)(c + 0x14) = *(unsigned short*)(c + 0x16);
    *(short*)(c + 0x12) = *(unsigned short*)(c + 0x14);
    int x = *(int*)c;
    if (x != 0) {
        if (x == 1) return;
        return;
    }
    func_ov007_020c3054(*(int**)(c + 0x24), *(unsigned short*)(c + 8), &mn0, &mx0);
    func_ov007_020c3054(*(int**)(c + 0x28), *(unsigned short*)(c + 8), &mn1, &mx1);
    *(short*)(c + 0xc) = mn0;
    *(short*)(c + 0xe) = mn1;
    *(short*)(c + 0x12) = mx0;
    *(short*)(c + 0x14) = mx1;
}
