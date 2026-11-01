/* storage 0x02380E10 runtime 0x037F8CA8 size 0xA8 wram. */

extern int *dat_03806E60[];     /* [3] = dword_3806E6C list head */
extern int dat_03806EB4;        /* sentinel node address */
extern int fn_02382AC4(void);   /* sub_37FA95C */
extern int fn_02382AD8(int);    /* sub_37FA970 */
extern int fn_02381384(int *);  /* sub_37F921C */
extern int fn_02380EC0(int);    /* sub_37F8D58 */

int fn_02380E10(int *a1, int a2) {
    int *v4 = dat_03806E60[3];
    int *v5 = 0;
    int v6 = fn_02382AC4();

    while (v4 != 0 && v4 != a1) {
        v5 = v4;
        v4 = (int *)v4[0x13];
    }
    if (v4 != 0 && v4 != &dat_03806EB4)
        goto work;
    fn_02382AD8(v6);
    return 0;
work:
    if (v4[0x15] != a2) {
        if (v5 == 0)
            dat_03806E60[3] = (int *)a1[0x13];
        else
            v5[0x13] = a1[0x13];
        a1[0x15] = a2;
        fn_02380EC0(fn_02381384(a1));
    }
    fn_02382AD8(v6);
    return 1;
}
