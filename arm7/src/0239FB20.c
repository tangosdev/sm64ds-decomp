/* storage 0x0239FB20 runtime 0x027D1A98 linked 0x70 main ARM.
 * Device entry dispatch: if the entry flag is set, refresh the handler
 * target and tail-call it. */
extern int *dat_0380FFF4;
typedef int (*HandlerFn)(void *, int);
int fn_0239FB20(int a1) {
    char *dev = (char *)dat_0380FFF4;
    char *base = dev + 0x3E4;
    int off = a1 * 20;
    char *e = base + off;
    unsigned short v = *(unsigned short *)(base + off);
    if (v == 0)
        return v;
    {
        void *p8 = *(void **)(e + 8);
        unsigned short w = *(unsigned short *)p8;
        if (w == 0) {
            *(unsigned short *)((char *)*(void **)(e + 0xC) + 8) = 2;
        } else {
            *(unsigned short *)((char *)*(void **)(e + 0xC) + 8) = w;
        }
        return ((HandlerFn)*(void **)(e + 0x10))(*(void **)(e + 0xC), 0);
    }
}
