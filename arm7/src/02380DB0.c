/* storage 0x02380DB0 runtime 0x037F8C48 size 0x54 wram. */

extern void fn_0238244C(void *);                      /* sub_37FA2E4 */
extern void fn_023822A8(void *, unsigned, int, void *, int); /* sub_37FA140 */
extern int fn_02381050(void *);                       /* sub_37F8EE8 */
extern int fn_02380DA4(int);                          /* sub_37F8C3C thunk */
extern int **dat_03806E58;

int fn_02380DB0(int a1) {
    char v3[44];

    fn_0238244C(v3);
    fn_023822A8(v3, ((unsigned)(a1 * 0x82EA)) >> 6, 0,
                (void *)fn_02380DA4, (int)*dat_03806E58);
    return fn_02381050(0);
}
