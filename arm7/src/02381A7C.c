/* storage 0x02381A7C runtime 0x037F9914 size 0xA4 wram ARM.
 * Aligned-alloc scan: lock, align base/size down to 32, find the first
 * negative entry in table a1 and fill it. One data pool word. */

extern int *unk_3806FA8[];
extern int fn_02382AC4(void);
extern int fn_02382AD8(int);

int fn_02381A7C(int a1, int a2, int a3) {
    int v6 = fn_02382AC4();
    int *v7 = unk_3806FA8[a1];
    int *v8 = (int *)((a2 + 31) & ~0x1f);
    int v9 = a3 & ~0x1f;
    int n;
    for (a1 = 0, n = v7[1]; a1 < n; ++a1) {
        int *v11 = (int *)(v7[4] + 12 * a1);
        if (*v11 < 0) {
            *v11 = v9 - (int)v8;
            *v8 = 0;
            v8[1] = 0;
            v8[2] = *v11;
            v11[1] = (int)v8;
            v11[2] = 0;
            fn_02382AD8(v6);
            return a1;
        }
    }
    fn_02382AD8(v6);
    return -1;
}
