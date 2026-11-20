/* storage 0x02381050 runtime 0x037F8EE8 size 0x64 wram. */

extern int **dat_03806E58;
extern int fn_02382AC4(void);   /* sub_37FA95C */
extern int fn_02382AD8(int);    /* sub_37FA970 */
extern void fn_02380EC0(void);  /* sub_37F8D58 */

int fn_02381050(unsigned short *a1) {
    int v2 = fn_02382AC4();
    int *v3 = *dat_03806E58;

    if (a1 != 0)
        *a1 |= (unsigned short)(1 << v3[0x14]);
    v3[0x12] = 0;
    fn_02380EC0();
    return fn_02382AD8(v2);
}
