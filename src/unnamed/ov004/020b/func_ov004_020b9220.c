extern unsigned char data_0208ee3c;
extern unsigned char data_0209d454;

void func_ov004_020b9220(char *c) {
    *(unsigned char *)(c + 0x58) = 1;
    if (data_0208ee3c != 0) {
        *(unsigned char *)(c + 0x7c) = 1;
        *(unsigned char *)(c + 0xa0) = 0;
    } else {
        *(unsigned char *)(c + 0x7c) = 0;
        *(unsigned char *)(c + 0xa0) = 1;
    }
    *(unsigned char *)(c + 0xc4) = 0;
    data_0209d454 |= 2;
    *(int *)(c + 0x120) = 8;
    *(unsigned char *)(c + 0x124) = 1;
}
