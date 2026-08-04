extern unsigned char data_ov006_02140990[];

void func_ov006_020d0b04(void) {
    int i = 0;
    unsigned char *p = data_ov006_02140990;
    int r0 = i;
    do {
        i++;
        *(p + 0x328) = r0;
        p += 0x32c;
    } while (i < 3);
}
