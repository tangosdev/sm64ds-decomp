/* storage 0x023A07BC runtime 0x027D2734 linked 0x2D8 main ARM.
 * Walk one wireless slot list. A stale entry fires its handler; a live
 * entry is claimed and posted to the MAC. */

extern int *dat_0380FFF4;
extern int fn_02380884(int);
extern int fn_023808C0(int);
extern int fn_02394B80(int);
extern int fn_0239D638(int, unsigned short *);
extern int fn_0239AAA8(unsigned short *);
extern int fn_02396EFC(unsigned int);
extern int fn_023A05A4(int, int);

#define POST(tag)                                                          \
    do {                                                                   \
        unsigned short val;                                                \
        char *mmio;                                                        \
        val = (unsigned short)((((unsigned)(id & 0x3FFF) << 15) >> 16) |   \
                               (tag));                                     \
        mmio = (char *)(0x4800000 + (chan << 2));                          \
        mmio += 0x8000;                                                    \
        *(unsigned short *)(mmio + 0xA0) = val;                            \
    } while (0)

void fn_023A07BC(int chan) {
    char *base;
    int token;
    int stride;
    int prev;
    int node;
    unsigned short *ent;
    unsigned short slot;
    int two;
    int arg;
    int id;
    char *blk;
    unsigned short *ctr;
    unsigned short *p;
    char *g;
    unsigned short *q;
    unsigned bits;

    base = (char *)dat_0380FFF4;
    if (*(unsigned short *)(base + chan * 12 + 0x160) == 0)
        return;
    token = fn_02380884(0x1000000);
    stride = chan * 20;
    base = (char *)dat_0380FFF4;
    if (*(unsigned short *)(base + stride + 0x3E4) != 0) {
        fn_023808C0(token);
        return;
    }
    node = *(int *)(base + chan * 12 + 0x158);
    two = 2;
    arg = 0;
    for (;;) {
        if (node == -1) {
            fn_023808C0(token);
            return;
        }
        prev = node;
        node = fn_02394B80(prev);
        ent = (unsigned short *)(prev + 0x10);
        slot = ent[1];
        base = (char *)dat_0380FFF4;
        if ((unsigned short)(*(int *)(base + 0x3A4) - ent[2])
            > *(unsigned short *)(base + 0x388)) {
            blk = (base + 0x3E4) + stride;
            ctr = (unsigned short *)(base + 0x494);
            *ctr = (unsigned short)(*ctr + 1);
            ent[4] = (unsigned short)two;
            p = (unsigned short *)(blk + 4);
            *p = (unsigned short)(*p + 1);
            ((void (*)(unsigned short *, int))*(int *)(blk + 0x10))(ent, arg);
            continue;
        }
        if (chan == 0) {
            bits = (unsigned)*(unsigned short *)(base + 0x4EC);
            if (((int)bits >> slot) & 1)
                ;
            else
                continue;
            if (*(unsigned short *)(*(char **)(base + 0x2E0) + slot * 0x1C) != 0x40) {
                ent[4] = (unsigned short)two;
                fn_0239D638((int)((char *)dat_0380FFF4 + 0x158),
                            (unsigned short *)((char *)ent - 0x10));
                fn_0239AAA8(ent);
                continue;
            }
        }
        *(unsigned short *)(base + stride + 0x3E4) = 1;
        g = (char *)dat_0380FFF4;
        q = (unsigned short *)(g + stride + 0x3E6);
        *q = (unsigned short)(*q + 1);
        g = (char *)dat_0380FFF4;
        *(unsigned short **)(g + stride + 0x3F0) = ent;
        g = (char *)dat_0380FFF4;
        id = *(int *)(g + stride + 0x3EC);
        if ((((unsigned)*(unsigned short *)(g + 0x316) << 30) >> 31) == 1)
            fn_02396EFC(2);
        fn_023A05A4(id, prev);
        g = (char *)dat_0380FFF4;
        if (*(unsigned short *)(g + 0x314) == 1) {
            char *row = *(char **)(g + 0x2E0) + slot * 0x1C;
            if (*(unsigned short *)(row + 0x16) > 1)
                *(unsigned short *)(id + 0xC) |= 0x2000;
        }
        bits = ent[0xA];
        if ((bits & 0xC) == 4)
            POST(0xA000);
        else if ((bits & 0xFC) == 0x50)
            POST(0x9000);
        else
            POST(0x8000);
        fn_023808C0(token);
        return;
    }
}
