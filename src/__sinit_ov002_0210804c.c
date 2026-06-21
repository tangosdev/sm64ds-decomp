/* __sinit_ov002_0210804c at 0x0210804c
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

extern int data_ov002_0211116c[3];
extern void func_020072c0(void);
extern int data_ov002_02111160;
extern void func_020731dc(void *a, void *b, void *c, int d);

void __sinit_ov002_0210804c(void) {
    data_ov002_0211116c[0] = 0x80000;
    data_ov002_0211116c[1] = 0;
    data_ov002_0211116c[2] = 0x60000;
    func_020731dc(data_ov002_0211116c, (void*)func_020072c0, &data_ov002_02111160, 0x60000);
}
