/* storage 0x0238F9BC runtime 0x027C1934 size 0x80 main. Command dispatcher:
 * blocking read on the +0x15C queue, u16 id bounds check, dispatch through
 * the 0x27-entry table at 0x027D63FC. 0238FA28 is this function's post-call
 * loop tail, not a separate function. */

extern int *fn_0238E688(void);
extern int fn_0238155C(int *obj, int *out, int flags);
extern void (*dat_27D63FC[])(unsigned int);

struct W { char pad0[0x15C]; int queue; char pad1[0x14B0]; unsigned char **reply; };

void fn_0238F9BC(void) {
    unsigned int msg;
    struct W *w;
    unsigned int id;
    unsigned char *v2;

    w = (struct W *)fn_0238E688();
    for (;;) {
        fn_0238155C(&w->queue, (int *)&msg, 1);
        id = *(unsigned short *)msg;
        if (id >= 0x27)
            continue;
        v2 = *w->reply;
        *(unsigned int *)(v2 + 4) = 1;
        v2 = *w->reply;
        *(unsigned short *)(v2 + 2) = (unsigned short)id;
        dat_27D63FC[id](msg);
        v2 = *w->reply;
        *(unsigned int *)(v2 + 4) = 0;
    }
}
