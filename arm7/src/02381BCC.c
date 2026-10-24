/* storage 0x02381BCC runtime 0x037F9A64 size 0x38 wram ARM.
 * Swap table a1's head word with a2 under lock. One data pool word. */

extern int *unk_3806FA8[];
extern int fn_02382AC4(void);
extern int fn_02382AD8(int);

int fn_02381BCC(int a1, int a2) {
    int v4 = fn_02382AC4();
    int *v5 = unk_3806FA8[a1];
    int v6 = *v5;
    *v5 = a2;
    fn_02382AD8(v4);
    return v6;
}
