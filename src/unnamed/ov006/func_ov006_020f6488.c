extern void func_ov004_020b56c8(char* p);
extern short data_ov004_020bf9e4;

void func_ov006_020f6488(char *c) {
    if (*((unsigned short *)(c + 0x5300 + 0xE2)) != 0) {
        unsigned short *ptr = (unsigned short *)((((int)c) + 0x53E2) & 0xFFFFFFFFFFFFFFFFULL);
        *ptr = *ptr - 1;
        if (*((unsigned short *)(c + 0x5300 + 0xE2)) != 0) {
            return;
        }
        if (*((unsigned char *)(c + 0x5000 + 0x405)) < *((unsigned short *)(c + 0x5300 + 0xEA))) {
            return;
        }
        func_ov004_020b56c8((char *)(5 * (5 - *((unsigned char *)(c + 0x5000 + 0x408)))));
        return;
    }
    if (data_ov004_020bf9e4 == 1) {
        *((unsigned short *)(c + 0x5300 + 0xE2)) = 0;
        *((int *)(c + 0x5000 + 0x3D8)) = 2;
    }
}
