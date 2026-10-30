extern void *fn_0238E688(void);
extern int fn_0239355C(int a, unsigned short b, unsigned short *c, int d, unsigned short *e, unsigned short *f);
extern unsigned short *fn_0238FF10(void *buf, unsigned short b, unsigned short c, int d);

void fn_02393C18(void) {
    unsigned short slot8;
    unsigned short slotA;
    unsigned short slotC;
    unsigned int buf[0x80];
    char *base = (char *)fn_0238E688();
    char *hi = base + 0x1000;
    char *obj = *(char **)(*(int *)(hi + 0x610));
    int field = *(int *)(obj + 0x50);
    if (fn_0239355C(1, *(unsigned short *)(obj + 0x18E), &slotC, field, &slot8, &slotA) == 0) {
        if (*(int *)(obj + 0x24) == 1)
            slotC |= 0x8000;
        unsigned short *ret = fn_0238FF10(buf, slotA, slotC, field);
        if (ret[2] == 0)
            *(unsigned short *)(obj + 0x8A) = 1;
    }
}
