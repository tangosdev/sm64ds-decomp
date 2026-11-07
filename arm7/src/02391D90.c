extern int *fn_0238E688(void);
extern void fn_02382C80(int val, void *dst, unsigned int n);
extern unsigned short *fn_0238E57C(unsigned short *result, unsigned short *a2);
extern unsigned short *fn_0238FF7C(unsigned short *a1, void *a2);
extern unsigned short *fn_0238DE4C(void);
extern int fn_0238DEF4(unsigned short *msg);

struct Rec91D90 {
    unsigned short a;
    unsigned char skip0[4];
    unsigned short b;
    unsigned char skip1[6];
    unsigned char flag;
    unsigned char skip2[9];
    unsigned short c[3];
    unsigned short d[3];
    unsigned short pad[4];
    int word;
};

int fn_02391D90(int *a1) {
    volatile unsigned short head;
    struct Rec91D90 rec;
    unsigned short big[256];
    char *base;
    char *hi;
    int **pp;
    int *obj;
    unsigned short sel;
    unsigned short *slot;
    unsigned short *msg;

    base = (char *)fn_0238E688();
    hi = base + 0x1000;
    pp = *(int ***)(hi + 0x610);
    obj = *pp;
    *(unsigned short *)((char *)obj + 0x9E) = (unsigned short)a1[1];
    *(unsigned short *)((char *)obj + 0xA0) = (unsigned short)a1[2];
    *(unsigned short *)((char *)obj + 0xA2) = (unsigned short)a1[3];
    *(int *)((char *)obj + 0xA4) = a1[4];
    *(unsigned short *)((char *)obj + 0xA8) = (unsigned short)a1[5];
    *(int *)((char *)obj + 0x20) = 1;
    head = 0;
    fn_02382C80(head, &rec, 0x30);
    rec.a = 0;
    rec.b = (unsigned short)a1[5];
    sel = *(unsigned short *)((char *)obj + 0x1E8);
    rec.flag = (unsigned char)(sel == 2 ? 0x14 : 0x0A);
    fn_0238E57C(rec.c, (unsigned short *)(a1 + 1));
    fn_0238E57C(rec.d, (unsigned short *)((char *)obj + 0xDC));
    rec.word = a1[4];
    slot = fn_0238FF7C(big, &rec);
    msg = fn_0238DE4C();
    msg[0] = 0x12;
    msg[1] = (int)((slot[2] == 0) ? (char *)0 : (char *)1);
    if (slot[2] != 0) {
        msg[2] = 0x100;
        msg[3] = slot[2];
    }
    return fn_0238DEF4(msg);
}
