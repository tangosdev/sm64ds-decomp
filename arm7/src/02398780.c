/* storage 0x02398780 runtime 0x027CA6F8 size 0x260 + pool 0x2A0 main ARM.
 * Wireless receive poll: gate on mode bits, walk the BB/RX frame with
 * fn_02398660, wait for the hardware counters to pass their thresholds,
 * latch the reply id, kick 0x48080B4, bump dat+0x3BA, toggle 0x4808244,
 * then fn_023985F4. */

typedef struct {
    char _000[0x60];
    unsigned short f_60[3];
    char _066[0x30];
    unsigned short f_96;
    char _098[0x22];
    unsigned short f_BA;
} Dat300;
typedef struct {
    char _000[0x86];
    unsigned short f_86;
    char _088[0x2];
    unsigned short f_8A;
} Dat400;
typedef struct {
    char _000[0x1C];
    unsigned short f_1C;
} Dat600;

unsigned fn_02398660(unsigned);
int fn_023985F4(void);

void fn_02398780(void) {
    int v4;
    int v1;
    unsigned int i;
    unsigned int v5;
    unsigned short *p;
    char *d;
    volatile unsigned short *regF8 = (volatile unsigned short *)0x48080F8;
    volatile unsigned short *reg268 = (volatile unsigned short *)0x4808268;

    *(volatile unsigned short *)0x4808010 = 0x40;
    d = *(char **)0x380FFF4;
    if (!(((Dat600 *)(d + 0x600))->f_1C & 0x20)) return;
    if (((Dat400 *)(d + 0x400))->f_8A != 0xFFFF) return;
    if ((*(volatile unsigned short *)0x480819C & 3) != 3) return;
    if (*(volatile unsigned short *)0x4808268 <
        (0xFFF & (int)((unsigned)*(volatile unsigned short *)0x4808050 >> 1)))
        return;
    v1 = *(volatile unsigned short *)0x4808054;
    p = (unsigned short *)fn_02398660(
            (unsigned)((unsigned short *)0x4804000 + v1) + 8);
    p = (unsigned short *)fn_02398660((unsigned)p + 4);
    if ((*p & 0xE7FF) != 0x228) return;
    p = (unsigned short *)fn_02398660((unsigned)p + 2);
    v4 = *regF8 - 0x10000;
    for (;;) {
        v5 = (unsigned short)(*reg268 - v1);
        if (v5 & 0x8000)
            v5 = (unsigned short)(v5 + ((unsigned)
                    *(unsigned short *)(*(char **)0x380FFF4 + 0x396) >> 1));
        if (v5 > 0xE) break;
        if ((unsigned short)(*regF8 - v4) > 0x40u) return;
    }
    p += 4;
    for (i = 0; i < 3; i++) {
        unsigned short *q = (unsigned short *)fn_02398660((unsigned)p);
        p = q + 1;
        if (*q != *(unsigned short *)(*(char **)0x380FFF4 + 0x360 + 2 * i))
            return;
    }
    p = (unsigned short *)fn_02398660((unsigned)p + 0xA);
    for (;;) {
        v5 = (unsigned short)(*reg268 - v1);
        if (v5 & 0x8000)
            v5 = (unsigned short)(v5 + ((unsigned)
                    *(unsigned short *)(*(char **)0x380FFF4 + 0x396) >> 1));
        if (v5 > 0x14) break;
        if ((unsigned short)(*regF8 - v4) > 0x70u) return;
    }
    {
        char *d2 = *(char **)0x380FFF4;
        if (*p & *(unsigned short *)(d2 + 0x486)) return;
        *(unsigned short *)(d2 + 0x48A) = *(volatile unsigned short *)0x4808098;
    }
    *(volatile unsigned short *)0x48080B4 = 0x40;
    ++*(unsigned short *)(*(char **)0x380FFF4 + 0x3BA);
    while ((*(volatile unsigned short *)0x480819C & 3) == 3) {}
    *(volatile unsigned short *)0x4808244 |= 0x40;
    *(volatile unsigned short *)0x4808244 &= ~0x40;
    *(volatile unsigned short *)0x4808228 = 8;
    *(volatile unsigned short *)0x4808228 = 0;
    fn_023985F4();
}
