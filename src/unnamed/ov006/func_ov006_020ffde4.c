#pragma opt_common_subs off
extern void func_ov006_020ff534(char *c, int k);
extern unsigned char data_ov006_0212ebe0[];
extern unsigned char data_ov006_0212ebd8[];
extern unsigned short data_ov006_0212ecac[];
extern unsigned short *data_ov006_0213daa4[];

void func_ov006_020ffde4(char *c, int k)
{
    if (*(unsigned char *)(c + k * 32 + 0x563b) != 0) {
        (*(unsigned short *)(c + 0x5630 + k * 32))++;
        if (*(unsigned short *)(c + k * 32 + 0x5630) >= data_ov006_0212ebe0[*(unsigned char *)(c + k * 32 + 0x5638)]) {
            *(unsigned short *)(c + k * 32 + 0x5630) = 0;
            (*(unsigned char *)(c + 0x5638 + k * 32))++;
            if (*(unsigned char *)(c + k * 32 + 0x5638) >= 8) {
                *(unsigned char *)(c + k * 32 + 0x5638) = 0;
                if (*(unsigned char *)(c + k * 32 + 0x563c) == 0) {
                    *(unsigned char *)(c + k * 32 + 0x5635) = 4;
                    *(unsigned char *)(c + k * 32 + 0x5637) = 0x10;
                    return;
                }
                (*(unsigned char *)(c + 0x563c + k * 32))--;
            }
        }
        *(unsigned char *)(c + k * 32 + 0x5637) = data_ov006_0212ebd8[*(unsigned char *)(c + k * 32 + 0x5638)];
    } else {
        (*(unsigned short *)(c + 0x5630 + k * 32))++;
        if ((unsigned int)*(unsigned short *)(c + k * 32 + 0x5630) >= (unsigned int)data_ov006_0212ecac[*(unsigned char *)(c + k * 32 + 0x5638)]) {
            *(unsigned short *)(c + k * 32 + 0x5630) = 0;
            (*(unsigned char *)(c + 0x5638 + k * 32))++;
            if (*(unsigned char *)(c + k * 32 + 0x5638) >= 7) {
                *(unsigned char *)(c + k * 32 + 0x5638) = 0;
            }
            *(unsigned char *)(c + k * 32 + 0x5637) = data_ov006_0213daa4[k][*(unsigned char *)(c + k * 32 + 0x5638)];
        }
        func_ov006_020ff534(c, k);
    }
}
