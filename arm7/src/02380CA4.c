/* storage 0x02380CA4 runtime 0x037F8B3C size 0x48 wram. */

extern int fn_02380B94(int, int, int, int); /* sub_37F8A2C */
extern void fn_02380B14(void);              /* sub_37F89AC delay helper */

int fn_02380CA4(int a1, int a2, int a3, int a4) {
    int r;

    while ((r = fn_02380B94(a1, a2, a3, a4)) > 0)
        fn_02380B14();
    return r;
}
