/* storage 0x02380EC0 runtime 0x037F8D58 size 0xB4 wram. */

typedef struct {
    unsigned short unk_00;          /* +0x00 */
    unsigned short unk_02;
    unsigned short unk_04;          /* +0x04 gate flag */
    unsigned short unk_06;
    int unk_08;                     /* +0x08 dword_3806E68 */
    int *unk_0C;                    /* +0x0C dword_3806E6C */
    void (*unk_10)(int, int);       /* +0x10 off_3806E70 */
} P3_03806E60;

extern P3_03806E60 dat_03806E60;
extern int **dat_03806E58;
extern void (*dat_03806E54)(int, int);
extern int fn_02382B1C(void);       /* sub_37FA9B4 */
extern int fn_02380F80(void);       /* sub_37F8E18 */
extern int fn_02381484(int *);      /* sub_37F931C */
extern int fn_023814B8(int);        /* sub_37F9350 */

int fn_02380EC0(void) {
    P3_03806E60 *w = &dat_03806E60;
    int *v2;
    int v3;
    int r;

    if (w->unk_04 != 0 || fn_02382B1C() == 0x12) {
        w->unk_00 = 1;
        return 1;
    }
    v2 = *dat_03806E58;
    v3 = fn_02380F80();
    if (v2 == (int *)v3)
        return v3;
    if (v3 == 0)
        return v3;
    r = fn_02381484(v2);
    if (r != 0)
        return r;
    if (dat_03806E54 != 0)
        dat_03806E54((int)v2, v3);
    if (w->unk_10 != 0)
        w->unk_10((int)v2, v3);
    dat_03806E60.unk_08 = v3;
    return fn_023814B8(v3);
}
