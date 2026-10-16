/* storage 0x02394988 runtime 0x027C6900 size 0x54 main ARM, no pool.
 * Walk the link chain through fn_02394B80 until fn_02394DF8 settles it. */
extern int fn_02394B80(int);
extern int fn_02394DF8(int, int);
void fn_02394988(int *a1) {
    int v = *a1;
    unsigned h = *(unsigned short *)((char *)a1 + 8);
    if (h == 0)
        return;
    while (v != -1) {
        int t = fn_02394B80(v);
        fn_02394DF8((int)a1, v);
        v = t;
    }
}
