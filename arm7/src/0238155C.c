/* storage 0x0238155C runtime 0x037F93F4 size 0xA4 wram. No pool.
 * The remainder of the signed division stays in r1. */

extern int fn_02382AC4(void);
extern void fn_02382AD8(int);
extern void fn_02381050(int);
extern void fn_02380FE0(int);

int fn_0238155C(int *obj, int *out, int flags) {
    int tok;
    int odd;
    int cursor;
    int *countp;

    tok = fn_02382AC4();
    odd = flags & 1;
    cursor = (int)obj + 2;
    while (*(int *)((char *)obj + 0x10) == 0) {
        if (odd == 0) {
            fn_02382AD8(tok);
            return 0;
        }
        fn_02381050(cursor);
    }
    if (out != 0) {
        int *table;
        int index;

        table = *(int **)((char *)obj + 4);
        index = *(int *)((char *)obj + 0xC);
        *out = table[index];
    }
    *(int *)((char *)obj + 0xC) =
        (*(int *)((char *)obj + 0xC) + 1) % *(int *)((char *)obj + 8);
    countp = (int *)((char *)obj + 0x10);
    *countp = *countp - 1;
    fn_02380FE0((int)obj);
    fn_02382AD8(tok);
    return 1;
}
