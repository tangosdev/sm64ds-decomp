/* storage 0x02381600 runtime 0x037F9498 size 0x98 wram. No pool. */

extern int fn_02382AC4(void);
extern void fn_02382AD8(int);
extern void fn_02381050(int);
extern void fn_02380FE0(int);

int fn_02381600(int *obj, int value, int flags) {
    int tok;
    int odd;
    int *countp;
    int idx;
    int *table;

    tok = fn_02382AC4();
    odd = flags & 1;
    while (*(int *)((char *)obj + 8) <= *(int *)((char *)obj + 0x10)) {
        if (odd == 0) {
            fn_02382AD8(tok);
            return 0;
        }
        fn_02381050((int)obj);
    }
    idx = (*(int *)((char *)obj + 0xC) + *(int *)((char *)obj + 0x10))
        % *(int *)((char *)obj + 8);
    table = *(int **)((char *)obj + 4);
    table[idx] = value;
    countp = (int *)((char *)obj + 0x10);
    *countp = *countp + 1;
    fn_02380FE0((int)obj + 2);
    fn_02382AD8(tok);
    return 1;
}
