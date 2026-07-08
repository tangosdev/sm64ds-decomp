extern void func_02016748(void *self, int anim, int a, int speed, int b);
extern int data_ov084_02130df4[];
extern void func_ov084_0212ef00(void *c);

void func_ov084_0212fc10(char *c)
{
    *(int *)(c + 0x80) = 0x1000;
    *(int *)(c + 0x84) = 0x1000;
    *(int *)(c + 0x88) = 0x1000;
    *(unsigned char *)(c + 0x45c) = 1;
    *(int *)(((long long)(int)(c + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x10000000;
    func_02016748(c + 0x110, data_ov084_02130df4[1], 0, 0x1000, 0);
    if (*(int *)(c + 0x464) < 0x4b0000)
        *(int *)(c + 0x458) = 1;
    func_ov084_0212ef00(c);
}
