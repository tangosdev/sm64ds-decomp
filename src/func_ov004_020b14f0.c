typedef long long s64;

#define AT(p,off) ((int*)(int)(((s64)(int)((char*)(p)+(off)))))

extern int func_ov004_020ad6f4(void* self, int a);
extern int func_ov004_020adc3c(void* self);
extern int func_02013580(int a, int b);
extern void func_ov004_020b1710(void* self, int a1, int a2, int a3);

void func_ov004_020b14f0(char* self)
{
    int a2 = 0x28;
    int n;
    int i;
    int j;
    int k;

    *AT(self, 0x60) += 1;
    if (*(int*)(self + 0x60) >= 0x8c)
        *(int*)(self + 0x60) = 0x64;

    n = func_ov004_020ad6f4(self, *(int*)(self + 0x4658));
    if (n < 0)
        goto neg;

    i = 0;
    if (n > 0)
    {
        do
        {
            if (*(int*)(self + 0x60) >= i * 0x14)
                func_ov004_020b1710(self, 0xa8, a2, func_02013580(func_ov004_020adc3c(self), i));
            a2 += 0x18;
            i++;
        } while (i < n);
    }

    if (*(int*)(self + 0x60) >= n * 0x14)
    {
        if (*(int*)(self + 0x60) < 0x64 || *(int*)(self + 0x60) - 0x64 < 0x14)
            func_ov004_020b1710(self, 0xa8, a2, *(int*)(self + 0x4658));
        else
            func_ov004_020b1710(self, 0xa8, a2, -1);
    }

    a2 += 0x18;
    j = n + 1;
    if (j >= 5)
        return;
    do
    {
        if (*(int*)(self + 0x60) >= j * 0x14)
            func_ov004_020b1710(self, 0xa8, a2, func_02013580(func_ov004_020adc3c(self), j));
        a2 += 0x18;
        j++;
    } while (j < 5);
    return;

neg:
    k = 0;
    do
    {
        if (*(int*)(self + 0x60) >= k * 0x14)
            func_ov004_020b1710(self, 0xa8, a2, func_02013580(func_ov004_020adc3c(self), k));
        a2 += 0x18;
        k++;
    } while (k < 5);

    if (*(int*)(self + 0x60) < 0x64)
        return;

    if (*(int*)(self + 0x60) - 0x64 < 0x14)
        func_ov004_020b1710(self, 0xa8, a2 + 0x10, *(int*)(self + 0x4658));
    else
        func_ov004_020b1710(self, 0xa8, a2 + 0x10, -1);
}
