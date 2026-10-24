/* storage 0x02381B24 runtime 0x037F99BC size 0xA4 wram ARM.
 * Table setup: publish arena, init all entries to (-1, 0, 0), align tail.
 * The entry stride (12 * a4) is hoisted by hand ahead of the loop.
 * One data pool word. */

extern int *unk_3806FA8[];
extern int fn_02382AC4(void);
extern int fn_02382AD8(int);

int fn_02381B24(int a1, int *a2, int a3, int a4) {
    int v8 = fn_02382AC4();
    int i;
    int v;
    unk_3806FA8[a1] = a2;
    v = 12 * a4;
    a2[4] = (int)(a2 + 5);
    a2[1] = a4;
    for (i = 0; i < a2[1]; ++i) {
        int *v10 = (int *)(a2[4] + 12 * i);
        *v10 = -1;
        v10[2] = 0;
        v10[1] = v10[2];
    }
    *a2 = -1;
    a2[2] = (a2[4] + v + 31) & ~0x1f;
    a2[3] = a3 & ~0x1f;
    fn_02382AD8(v8);
    return a2[2];
}
