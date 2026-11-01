/* storage 0x02395CB8 runtime 0x027C7C30 size 0xF0 main.
 * Reset/io-quiesce routine: two init calls, clears the +0x3A0 state halfword,
 * clears bit 2 at +0x316, zeroes six 0x48080xx MMIO halfwords, runs the
 * case-1 hook when the +0x314 mode halfword is 1, sets two MMIO halfwords
 * to 0xFFFF, then runs two finalizers. Pool: 0x0380FFF4, 0x316, eight
 * MMIO addrs, 0xFFFF. */

extern void fn_02395640(void); /* =0x027C75B8 */
extern int fn_023969F4(int a1); /* =0x027C896C */
extern void fn_0239F664(void); /* =0x027D15DC */
extern void fn_0239F890(void); /* =0x027D1808 */
extern void fn_023949DC(void); /* =0x027C6954 */

void fn_02395CB8(void) {
    fn_02395640();
    fn_023969F4(32);
    *(unsigned short *)(*(unsigned int *)0x0380FFF4 + 0x300 + 0xA0) = 0;
    *(unsigned short *)(*(unsigned int *)0x0380FFF4 + 0x316) &= ~4;
    *(volatile unsigned short *)0x4808012 = 0;
    *(volatile unsigned short *)0x4808004 = 0;
    *(volatile unsigned short *)0x48080EA = 0;
    *(volatile unsigned short *)0x48080E8 = 0;
    *(volatile unsigned short *)0x4808008 = 0;
    *(volatile unsigned short *)0x480800A = 0;
    switch (*(unsigned short *)(*(unsigned int *)0x0380FFF4 + 0x300 + 0x14)) {
    case 1:
        fn_0239F664();
        break;
    case 0:
    case 2:
    case 3:
    case 4:
        break;
    }
    *(volatile unsigned short *)0x48080AC = 0xFFFF;
    *(volatile unsigned short *)0x48080B4 = 0xFFFF;
    fn_0239F890();
    fn_023949DC();
}
