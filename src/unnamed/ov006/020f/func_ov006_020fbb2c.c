extern int func_ov004_020adbc0(void);
extern void func_ov004_020adb1c(int self);

void func_ov006_020fbb2c(char *c, int idx, unsigned short val)
{
    int i;
    char *p = c + 0x15c;
    for (i = 0x1d; i >= 0; p -= 0xc, i--) {
        if (*(unsigned short *)(p + 0x4cfa) != 0) continue;
        *(int *)(c + i * 0xc + 0x4cf0) = *(int *)(c + idx * 0x14 + 0x5958);
        *(int *)(c + i * 0xc + 0x4cf4) = *(int *)(c + idx * 0x14 + 0x595c);
        *(unsigned short *)(c + i * 0xc + 0x4cfa) = 0x30;
        *(unsigned short *)(c + 0x4cf8 + i * 0xc) = val;
        func_ov004_020adb1c(*(unsigned short *)(c + 0x4cf8 + i * 0xc) + func_ov004_020adbc0());
        if ((unsigned int)func_ov004_020adbc0() < 0xbb8) return;
        if (*(unsigned char *)(c + 0xc3) != 0)
            *(unsigned char *)(c + 0xc3) = 0;
        return;
    }
}
