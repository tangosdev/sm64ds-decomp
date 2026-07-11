extern int __aeabi_idiv(int a, int b);

void func_ov006_02106080(char* self, int x)
{
    int W = *(int*)(self + 0x4cbc);
    int n1 = 3;
    int n2 = 3;
    int a = x % W;
    int b = x / W;
    if (a == 0 || a == W - 1) n1 = 2;
    if (b == 0 || b == W - 1) n2 = 2;
    a = a - 1;
    if (a < 0) a = 0;
    b = b - 1;
    if (b < 0) b = 0;
    int i, j;
    for (i = 0; i < n2; i++) {
        for (j = 0; j < n1; j++) {
            unsigned char* p = (unsigned char*)((((int)self + ((*(int*)(self + 0x4cbc)) * (b + i) + (a + j))) + 0x4f1e) & 0xFFFFFFFFFFFFFFFF);
            *p ^= 1;
        }
    }
}
