/* storage 0x02380D70 runtime 0x037F8C08 size 0x30 wram. */

extern int dat_03806E60[];      /* [4] = off_3806E70 slot */
extern int fn_02382AC4(void);   /* sub_37FA95C */
extern int fn_02382AD8(int);    /* sub_37FA970 */

int fn_02380D70(int a1) {
    int v2 = fn_02382AC4();
    int v3 = dat_03806E60[4];

    dat_03806E60[4] = a1;
    fn_02382AD8(v2);
    return v3;
}
