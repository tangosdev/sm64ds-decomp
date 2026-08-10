extern int func_0205cdf4(char* self, int n);

int func_0205d5e8(char* self, int a1, int a2, int a3, int a4)
{
    int* p;
    *(int*)(self + 8) = a1;
    *(int*)(self + 0x34) = a4;
    *(int*)(self + 0x2c) = a2;
    *(int*)(self + 0x30) = a3;
    if(func_0205cdf4(self, 7) == 0) return 0;
    p = (int*)(self + 0xc);
    *p |= 0x10;
    *p &= ~0x20;
    return 1;
}
