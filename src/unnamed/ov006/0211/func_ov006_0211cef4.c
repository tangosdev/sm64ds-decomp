void func_ov006_0211cef4(char *c, int i)
{
    unsigned short *p16 = (unsigned short *)(c + 0x4bdc + i * 0x1c);
    int *sum;
    int *add;
    int v;
    *p16 = *p16 + 1;
    if (*p16 >= 8) {
        unsigned char *p8;
        int z = 0;
        *p16 = z;
        p8 = (unsigned char *)(c + 0x4be3 + i * 0x1c);
        *p8 = *p8 + 1;
        if (*p8 >= 4) *p8 = z;
    }
    add = (int *)(c + 0x4bd4 + i * 0x1c);
    sum = (int *)(c + 0x4bcc + i * 0x1c);
    *sum = *sum + *add;
    c = c + i * 0x1c;
    v = *sum >> 12;
    if (*(unsigned char *)(c + 0x4be4) != 0) {
        if (v >= 0x120) {
            *(unsigned char *)(c + 0x4be1) = 6;
            return;
        }
        if (*add <= 0x3000) *add = *add + 0x200;
    } else {
        if (v <= -0x20) {
            *(unsigned char *)(c + 0x4be1) = 6;
            return;
        }
        if (*add >= -0x3000) *add = *add - 0x200;
    }
}
