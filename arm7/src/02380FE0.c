/* storage 0x02380FE0 runtime 0x037F8E78 size 0x6C wram. */

extern int *dat_03806E60[];     /* [3] = dword_3806E6C list head */
extern int fn_02382AC4(void);   /* sub_37FA95C */
extern int fn_02382AD8(int);    /* sub_37FA970 */
extern void fn_02380EC0(void);  /* sub_37F8D58 */

int fn_02380FE0(unsigned short *a1) {
    int v2 = fn_02382AC4();
    unsigned int v3 = *a1;
    int *i;

    if (v3 != 0) {
        i = dat_03806E60[3];
        while (i != 0) {
            if ((v3 & (1 << i[0x14])) != 0)
                i[0x12] = 1;
            i = (int *)i[0x13];
        }
        *a1 = 0;
        fn_02380EC0();
    }
    return fn_02382AD8(v2);
}
