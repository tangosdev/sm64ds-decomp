/* storage 0x023814E4 runtime 0x037F937C size 0x78 wram. No pool. */

extern int fn_02382AC4(void);
extern void fn_02382AD8(int);
extern void fn_02381050(int);

int fn_023814E4(int *obj, int *out, int flags) {
    int tok;
    int odd;
    int cursor;

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
    fn_02382AD8(tok);
    return 1;
}
