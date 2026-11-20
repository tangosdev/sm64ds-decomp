/* storage 0x02389A04 runtime 0x0380189C size 0x64 + 4 pool wram ARM.
 * One pool data ref (0x03805E84), BLs to in-pack fn_02389848/74 (848 stays
 * asm) and out-of-pack fn_02382B38. Switch with shared store-and-return-
 * address tail; cases in source order 1, 3, 2 (the compiler lays blocks
 * out in source order). The pool address is taken at the tail, not homed. */

extern int fn_02389848(int a1);
extern int fn_02389874(int a1);
extern int fn_02382B38(void);
extern int dword_3805E84;

int *fn_02389A04(int a1) {
    int save = a1;
    switch (save) {
    case 1:
        fn_02389848(16);
        break;
    case 3:
        fn_02389874(48);
        break;
    case 2:
        fn_02389848(32);
        fn_02389874(16);
        break;
    default:
        fn_02382B38();
        break;
    }
    {
        int *r = &dword_3805E84;
        *r = save;
        return r;
    }
}
