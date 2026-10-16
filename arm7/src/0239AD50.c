/* storage 0x0239AD50 runtime 0x027CCCC8 size 0xAC main. */

int fn_0239AD50(unsigned short *a1) {
    unsigned char *g;
    unsigned char *e;
    unsigned int i, used;
    unsigned short key;
    unsigned int cnt, lim;

    key = a1[0];
    if (key & 1)
        return 0;
    g = *(unsigned char **)0x0380FFF4;
    cnt = *(unsigned short *)(g + 0x4E8);
    if (cnt > 1) {
        e = *(unsigned char **)(g + 0x2E0) + 0x1C;
        used = 0;
        i = 1;
        lim = *(unsigned short *)(g + 0x2E6);
        for (; i < lim; i++, e += 0x1C) {
            if (*(unsigned short *)e == 0)
                continue;
            if (*(unsigned short *)(e + 8) == a1[2] &&
                *(unsigned short *)(e + 6) == a1[1] &&
                *(unsigned short *)(e + 4) == key)
                return i;
            if (++used >= cnt)
                break;
        }
    }
    return 0xFF;
}
