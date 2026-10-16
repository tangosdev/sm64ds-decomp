extern void fn_02386958(void *a, void *b, void *c, void *d);
extern void fn_02385400(void *dst, void *src, unsigned int len);
int fn_0239DC24(unsigned char *a1, unsigned short *a2) {
    unsigned char *base;
    unsigned char *p308;
    unsigned char *p2e0;
    int k;
    unsigned int len;
    int off;
    unsigned char *slot;
    unsigned short *rec;
    unsigned int addr;
    unsigned short *aligned;
    base = *(unsigned char **)0x0380FFF4;
    p308 = base + 0x308;
    p2e0 = base + 0x2E0;
    k = 1;
    a2[1] = (unsigned short)k;
    base = *(unsigned char **)0x0380FFF4;
    if (*(unsigned short *)(base + 0x2F2) != 2)
        return 11;
    len = *(unsigned short *)(a1 + 0x10);
    if (len > 0x204)
        return 5;
    if (*(unsigned short *)(base + 0x434) == 0)
        k = 0;
    off = k * 20;
    slot = base + off;
    if (*(unsigned short *)(slot + 0x434) != 0)
        return 8;
    if (*(unsigned short *)0x04808094 & 0x8000)
        return 8;
    rec = *(unsigned short **)(slot + 0x43C);
    rec[0] = 0;
    rec[2] = 0;
    rec[4] = 20;
    rec[5] = (unsigned short)(*(unsigned short *)(a1 + 0x10) + 30);
    rec[6] = 0x118;
    fn_02386958(rec + 8, p308 + 0x58, p2e0 + 8, (void *)0x027D64FC);
    rec[18] = *(unsigned short *)(a1 + 0x12);
    len = *(unsigned short *)(a1 + 0x10);
    if (len != 0)
        fn_02385400((unsigned char *)rec + 0x26, *(void **)(a1 + 0x14), len);
    if (*(unsigned short *)(*(unsigned char **)0x0380FFF4 + 0x61C) & 4) {
        len = *(unsigned short *)(a1 + 0x10);
        len = len + 2;
        addr = (unsigned int)((unsigned char *)rec + 0x24) + len;
        addr = addr + 3;
        aligned = (unsigned short *)(addr & ~3u);
        aligned[0] = 0xB6B8;
        aligned[1] = 0x1D46;
    }
    *(unsigned short *)(*(unsigned char **)0x0380FFF4 + off + 0x434) = 1;
    {
        unsigned short *ctr;
        ctr = (unsigned short *)(*(unsigned char **)0x0380FFF4 + off + 0x436);
        *ctr = (unsigned short)(*ctr + 1);
    }
    *(unsigned short *)0x04808094 = (unsigned short)((((unsigned int)rec & 0x3FFF) >> 1) | 0x8000);
    return 0;
}
