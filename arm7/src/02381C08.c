/* storage 0x02381C08 runtime 0x037F9AA0 size 0x70 wram ARM.
 * Entry update: unlink then reinsert the a2-th slot through the two
 * in-pack list helpers. One data pool word. */

extern int *unk_3806FA8[];
extern int fn_02382AC4(void);
extern int fn_02382AD8(int);
extern int *fn_02381D94(int *r, int *p);
extern int fn_02381E50(int a1, int *p);

int fn_02381C08(int a1, int a2, int a3) {
    int v8;
    int *v7;
    int v6;
    v6 = fn_02382AC4();
    v7 = unk_3806FA8[a1];
    if (a2 < 0)
        a2 = *v7;
    v8 = a3 - 32;
    a1 = v7[4] + 12 * a2;
    *(int *)(a1 + 8) = fn_02381E50(*(int *)(a1 + 8), (int *)v8);
    *(int *)(a1 + 4) = (int)fn_02381D94(*(int **)(a1 + 4), (int *)v8);
    return fn_02382AD8(v6);
}
