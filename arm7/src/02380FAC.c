/* storage 0x02380FAC runtime 0x037F8E44 size 0x34 wram. */

extern int fn_02382AC4(void);   /* sub_37FA95C */
extern int fn_02382AD8(int);    /* sub_37FA970 */
extern void fn_02380EC0(void);  /* sub_37F8D58 */

int fn_02380FAC(int *a1) {
    int v2 = fn_02382AC4();

    a1[0x12] = 1;               /* +0x48 */
    fn_02380EC0();
    return fn_02382AD8(v2);
}
