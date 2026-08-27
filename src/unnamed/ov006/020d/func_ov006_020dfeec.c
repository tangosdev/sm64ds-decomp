#pragma opt_strength_reduction off
#pragma opt_common_subs off
extern void func_ov004_020adb1c(int self);
extern void func_ov006_020c2924(char *c);
extern void func_ov004_020b0cac(int a, int b, int c, int d, int e, int f);

void func_ov006_020dfeec(char *c, int msg)
{
    int i;

    if (msg == 3 || msg == 0x12) {
        if (msg == 3) {
            *(int *)(c + 0xb4) = 0;
        } else {
            *(int *)(c + 0xb4) = 0;
            func_ov004_020adb1c(*(int *)(c + 0xb4));
        }
        *(int *)(c + 0xa8) = 2;
        *(int *)(c + 0xac) = *(int *)(c + 0xa8);
        *(unsigned char *)(c + 0x5000 + 0x462) = 1;
        *(unsigned char *)(c + 0x5000 + 0x463) = 0;
        if (*(int *)(c + 0xb4) >= 3) {
            *(unsigned char *)(c + 0x5000 + 0x464) = 2;
        } else {
            *(unsigned char *)(c + 0x5000 + 0x464) = 0;
        }
        *(unsigned char *)(c + 0x5000 + 0x468) = 1;
    } else if (msg == 0) {
        *(int *)(c + 0xa8) = 2;
        *(int *)(c + 0xac) = *(int *)(c + 0xa8);
        if (*(int *)(c + 0xb4) == 3) {
            *(unsigned char *)(c + 0x5000 + 0x462) = 1;
            *(unsigned char *)(c + 0x5000 + 0x463) = 0;
            *(unsigned char *)(c + 0x5000 + 0x464) = 2;
        }
    }

    for (i = 0; i < 3; i++) {
        if (*(unsigned char *)(c + i + 0x5000 + 0x462) != 0) {
            *(int *)(c + i * 4 + 0x5000 + 0x434) = 3;
        } else {
            *(int *)(c + i * 4 + 0x5000 + 0x434) = 0;
        }
        *(int *)(c + i * 4 + 0x5000 + 0x440) = 0;
        *(int *)(c + i * 4 + 0x5000 + 0x44c) = 0;
    }

    *(int *)(c + 0x5000 + 0x41c) = 0x3c;
    *(int *)(c + 0x5000 + 0x418) = 0;
    *(int *)(c + 0x5000 + 0xdc) = 0;
    func_ov006_020c2924(c + 0x4f38);

    *(unsigned int *)(c + 0xbc) = *(int *)(c + 0xb4);
    if (*(unsigned int *)(c + 0xbc) > 0x270e) {
        *(unsigned int *)(c + 0xbc) = 0x270e;
    }
    func_ov004_020b0cac(0xd, 0x80, 0xa0, 1, -1, 0xd);
}
