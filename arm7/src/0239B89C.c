extern int fn_02395C74(int a1);
extern void fn_02397738(unsigned short a1, unsigned short a2);
extern int fn_02395334(void *p);
extern int fn_023A43D8(int a1);
extern void fn_023970C8(unsigned short a1, int a2);
extern void fn_02395DA8(void);
extern void fn_02396EE8(int a1);
extern void fn_0239554C(unsigned short a1, int a2);
extern int fn_0238D7A0(int a1, int a2);
extern void fn_02395CB8(void);
extern void fn_02395BE0(int a1, unsigned short a2);
extern void fn_0239B7E8(void);
extern void fn_0239477C(int a1, int a2);

void fn_0239B89C(void) {
    unsigned short *st;
    int *g;
    int ret;
    unsigned short idx;
    int *obj;
    unsigned int n;
    unsigned int d;
    unsigned int q;
    unsigned int i;

    st = (unsigned short *)((char *)*(int **)0x0380FFF4 + 0x3BC);
    switch (*st) {
    case 0x80:
        *(unsigned short *)((char *)st + 0x14) = 0;
        *(unsigned short *)((char *)st + 0xE) = (unsigned short)fn_02395C74(0x13);
        *(unsigned short *)((char *)st + 0x10) = (unsigned short)fn_02395C74(0x35);
        obj = *(int **)((char *)st + 0x18);
        fn_02397738(*(unsigned short *)((char *)obj + 0x12),
                    *(unsigned short *)((char *)obj + 0x14));
        g = *(int **)0x0380FFF4;
        *(unsigned short *)((char *)g + 0x314) = 4;
        *(unsigned short *)((char *)st + 0x16) = 0;
    case 0x81:
        {
            int **pp = (int **)0x0380FFF4;
            *(int *)((char *)*pp + 0x3C0) = 0;
            *(int *)((char *)*pp + 0x3C4) = 0;
        }
        obj = *(int **)((char *)st + 0x18);
        ret = fn_02395334((char *)obj + 0x18 + *(unsigned short *)((char *)st + 0x14));
        if (ret == 0)
            goto fail84;
        idx = *(unsigned short *)((char *)st + 0x14);
        if (idx >= 0x10) {
        fail84:
            *st = 0x84;
            break;
        }
        if (fn_023A43D8(0) != 0) {
            *(unsigned short *)((char *)st + 0x16) = 0xE;
            g = *(int **)0x0380FFF4;
            *(unsigned short *)((char *)g + 0x3BC) = 0x84;
            break;
        }
        if (*st == 0x80) {
            fn_023970C8((unsigned short)ret, 0);
            fn_02395DA8();
            *(unsigned short *)((char *)st + 0xC) = *(unsigned short *)0x04808040;
            fn_02396EE8(0x8000);
        } else {
            fn_023970C8((unsigned short)ret, 0);
        }
        *st = 0x82;
        obj = *(int **)((char *)st + 0x18);
        fn_0239554C(*(unsigned short *)((char *)obj + 0x16), 0x027CD7E8);
    case 0x82:
        {
            int *counter = (int *)((char *)st + 4);
            *counter += 1;
        }
        if (*(unsigned short *)0x0480819C & 1) {
            int *step = (int *)((char *)st + 8);
            *step += 100;
        }
        break;
    case 0x83:
        obj = *(int **)((char *)st + 0x18);
        ret = fn_02395334((char *)obj + 0x18 + *(unsigned short *)((char *)st + 0x14));
        n = *(unsigned int *)((char *)st + 4);
        if (n != 0) {
            d = *(unsigned int *)((char *)st + 8);
            if (d != 0) {
                q = (unsigned int)fn_0238D7A0((int)d, (int)n) + 1;
                if (q > 100)
                    q = 100;
            } else {
                q = 0;
            }
            obj = *(int **)((char *)st + 0x1C);
            idx = *(unsigned short *)((char *)st + 0x14);
            *(unsigned short *)((char *)obj + idx * 2 + 8) =
                (unsigned short)(ret | (q << 8));
        } else {
            obj = *(int **)((char *)st + 0x1C);
            idx = *(unsigned short *)((char *)st + 0x14);
            *(unsigned short *)((char *)obj + idx * 2 + 8) = (unsigned short)ret;
        }
        {
            unsigned short *hp = (unsigned short *)((char *)st + 0x14);
            *hp = (unsigned short)(*hp + 1);
        }
        *st = 0x81;
        break;
    case 0x84:
        fn_02395CB8();
        g = *(int **)0x0380FFF4;
        *(unsigned short *)((char *)g + 0x314) =
            *(unsigned short *)((char *)g + 0x2F2);
        fn_02395BE0(0x13, *(unsigned short *)((char *)st + 0xE));
        fn_02395BE0(0x35, *(unsigned short *)((char *)st + 0x10));
        fn_02396EE8(*(unsigned short *)((char *)st + 0xC));
        obj = *(int **)((char *)st + 0x1C);
        *(unsigned short *)((char *)obj + 4) = *(unsigned short *)((char *)st + 0x16);
        *st = 0;
        for (i = *(unsigned short *)((char *)st + 0x14); i < 0x10; i++) {
            obj = *(int **)((char *)st + 0x1C);
            *(unsigned short *)((char *)obj + i * 2 + 8) = 0;
        }
        fn_0239B7E8();
        break;
    default:
        break;
    }
    if (*st != 0)
        fn_0239477C(2, 5);
}
