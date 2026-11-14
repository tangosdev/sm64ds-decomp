extern int fn_02395334(void *p);
extern int fn_02395238(unsigned char byte, unsigned short acc);

int fn_023A43D8(int *a1) {
    unsigned int acc;
    unsigned char *cursor;
    unsigned int len;
    int *base;
    unsigned short *rec;
    int raw;
    unsigned short hdr;

    base = *(int **)0x0380FFF4;
    rec = *(unsigned short **)((char *)base + 0x2DC);
    len = rec[1];
    if (len < 0xA4 || len > 0x1D6)
        return 2;
    cursor = (unsigned char *)rec + 2;
    acc = 0;
    while (len != 0) {
        raw = fn_02395334(cursor);
        cursor += 1;
        acc = (unsigned int)fn_02395238((unsigned char)raw, (unsigned short)acc);
        len -= 1;
    }
    base = *(int **)0x0380FFF4;
    rec = *(unsigned short **)((char *)base + 0x2DC);
    hdr = rec[0];
    if (a1 != 0)
        *a1 = (unsigned short)hdr | (acc << 16);
    return acc != (unsigned int)hdr;
}
