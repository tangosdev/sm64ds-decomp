extern void *fn_0238E688(void);
extern int fn_02382AC4(void);
extern void fn_02382AD8(int saved);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *msg);

void fn_023943C4(int *a1) {
    char *base = (char *)fn_0238E688();
    char *hi = base + 0x1000;
    unsigned short *obj = *(unsigned short **)(*(int *)(hi + 0x610));
    unsigned short value = (unsigned short)a1[1];
    if (value == 0)
        value = 0x10;
    int saved = fn_02382AC4();
    obj[0x1E] = value;
    if (obj[0x1F] > value)
        obj[0x1F] = value;
    fn_02382AD8(saved);
    unsigned short *msg = fn_0238DE4C();
    msg[0] = 0x23;
    msg[1] = 0;
    fn_0238DEF4(msg);
}
