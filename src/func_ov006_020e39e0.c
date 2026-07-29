extern int func_ov004_020adbc0(void);
extern void func_ov004_020adb1c(int self);

void func_ov006_020e39e0(char *c, int a, int b)
{
    int i;
    char *p;
    int sx;
    int sy;
    unsigned char *q;

    p = c;
    for (i = 0; i < 0x3c; i++, p += 0x18) {
        if (*(unsigned char *)(p + 0x4ff4) != 0) continue;

        sx = *(int *)(c + a * 0x30 + 0x4660) + *(int *)(c + 0x4000 + b * 0x30 + 0x660);
        sy = *(int *)(c + a * 0x30 + 0x4664) + *(int *)(c + 0x4000 + b * 0x30 + 0x664);

        *(unsigned char *)(c + i * 0x18 + 0x4ff4) = 1;
        *(unsigned char *)(c + 0x4ff5 + i * 0x18) = 1;
        *(int *)(c + i * 0x18 + 0x4fe0) = sx >> 1;
        *(int *)(c + i * 0x18 + 0x4fe4) = sy >> 1;
        *(unsigned short *)(c + i * 0x18 + 0x4ff0) = 0x40;
        *(int *)(c + i * 0x18 + 0x4fe8) = 0;
        *(int *)(c + i * 0x18 + 0x4fec) = 0;

        if (*(unsigned char *)(c + a * 0x30 + 0x468d) != 0 && *(unsigned char *)(c + 0x4000 + b * 0x30 + 0x68d) != 0) {
            *(unsigned short *)(c + 0x4ff2 + i * 0x18) = (*(unsigned char *)(c + 0x55bf) + 1) * 10;
        } else if (*(unsigned char *)(c + a * 0x30 + 0x468d) != 0 || *(unsigned char *)(c + 0x4000 + b * 0x30 + 0x68d) != 0) {
            *(unsigned short *)(c + 0x4ff2 + i * 0x18) = (*(unsigned char *)(c + 0x55bf) + 1) * 10;
        } else {
            *(unsigned short *)(c + 0x4ff2 + i * 0x18) = (*(unsigned char *)(c + 0x55bf) + 1) * 100;
            *(unsigned char *)(c + 0x4ff5 + i * 0x18) = 2;
        }

        q = (unsigned char *)(c + 0x55bf);
        *q = *q + 1;
        if (*(unsigned char *)(c + 0x55bf) >= 0x17) *(unsigned char *)(c + 0x55bf) = 0x17;
        func_ov004_020adb1c(*(unsigned short *)(c + 0x4ff2 + i * 0x18) + func_ov004_020adbc0());
        return;
    }
}
