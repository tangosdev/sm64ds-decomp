extern unsigned char data_ov066_0211ae0c;

extern int func_ov066_021168ec(void *c);
extern void func_ov066_021162e8(int *c);
extern void func_ov066_0211632c(char *self);
extern void func_ov066_02116ac4(char *c, int strength);

int func_ov066_021184e0(char *c)
{
    if (func_ov066_021168ec(c) != 0 && func_ov066_021168ec(c) != 4) {
        *(int *)(c + 0xb0) = 0;
        func_ov066_021162e8((int *)c);
        return 1;
    }
    func_ov066_0211632c(c);
    switch (*(int *)(c + 0x4a0)) {
    case 0:
        if (data_ov066_0211ae0c == *(int *)(c + 0x49c)) {
            int *p = (int *)((long long)(int)(c + 0x4a0));
            *(int *)(c + 0x9c) = -0x14000;
            *(int *)(c + 0xa8) = 0x64000;
            *p = *p + 1;
        }
        break;
    case 1:
        if (*(int *)(c + 0x9c) != 0) {
            if (*(int *)(c + 0x4a8) >= *(int *)(c + 0x60)) {
                *(int *)(c + 0x60) = *(int *)(c + 0x4a8);
                *(int *)(c + 0xa8) = 0;
                *(int *)(c + 0x9c) = 0;
                func_ov066_02116ac4(c, 0x7d0000);
                if ((data_ov066_0211ae0c & *(int *)(c + 0x49c)) != 0)
                    data_ov066_0211ae0c ^= *(int *)(c + 0x49c);
                *(int *)(c + 0x4a0) = 0;
            }
        }
        break;
    }
    return 1;
}
