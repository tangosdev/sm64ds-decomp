/* storage 0x0239483C runtime 0x027C67B4 size 0xB4 + pool 0xC0 main ARM.
 * Event loop: probe, check, dispatch the pending slot through its +148
 * callback, then re-mark it pending (0xFFFF) and loop. */
extern int dword_380FFF4[];
extern int fn_0238155C(int, int *, int);
extern int fn_02394680(int *);
extern int fn_02380884(int);
extern void fn_023808C0(int);
extern int fn_02394704(int);
void fn_0239483C(void) {
    unsigned short *v0;
    int *pp = (int *)dword_380FFF4;
    v0 = *(unsigned short **)pp;
    int lim = 0xFFFF;
    int z = 0;
    int v5;
    v0[8] = z;
    v0[10] = z;
    while (1) {
        if (fn_0238155C(*(int *)((char *)*pp + 716), &v5, z) != 0)
            fn_02394680(&v5);
        {
            int v4 = fn_02380884(0x1000010);
            v0[9] = v0[8];
            if (v0[v0[9]] == lim) {
                v0[8]++;
                fn_023808C0(v4);
                continue;
            }
            fn_023808C0(v4);
            v0[10] = fn_02394704(v0[9]);
            ((void (*)(void))*(int *)((char *)v0 + v0[10] * 8 + 148))();
            v0[10] = lim;
        }
    }
}
