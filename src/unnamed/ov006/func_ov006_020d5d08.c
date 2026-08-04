#pragma opt_strength_reduction off
void func_ov006_020d5d08(char *c)
{
    int i;
    for (i = 0; i < 2; i++) {
        char *b = c + (i << 4);
        if (*(unsigned char *)(b + 0x6000 + 0x2bc)) {
            unsigned short *h = (unsigned short *)(((int)b + 0x62b8));
            *h = *h + 1;
            if (*h >= 4) {
                unsigned char *p;
                *h = 0;
                p = (unsigned char *)(((int)b + 0x62be));
                *p = *p + 1;
                *p = *p & 3;
            }
        }
    }
}
