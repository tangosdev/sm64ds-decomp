/* storage 0x02396A98 runtime 0x027C8A10 size 0xD0 main.
 * Spin on MMIO status bits, clear two MMIO halfwords, then run the
 * pending-length handler. Five pool refs, two calls. */

extern unsigned char *off_380FFF4; /* =0x0380FFF4 */
extern void fn_0239F9AC(void); /* =0x027D1924 */
extern void fn_0239A98C(unsigned int a1, unsigned int a2); /* =0x027CC904 */

void fn_02396A98(void) {
    unsigned int zero = 0;
    unsigned char *b = off_380FFF4 + 0x300;
    volatile unsigned short *p8214 = (volatile unsigned short *)0x4808214;
    volatile unsigned short *p819C = (volatile unsigned short *)0x480819C;
    *(unsigned short *)(b + 0x66) = zero;
    *(unsigned short *)(off_380FFF4 + 0x400 + 0x86) = zero;
    do {
        while (*p819C & 1)
            ;
    } while (*p8214 == 6 || *p8214 == 5);
    *(volatile unsigned short *)0x4808028 = zero;
    *(volatile unsigned short *)0x480802A = zero;
    if (*(unsigned short *)(off_380FFF4 + 0x300 + 0x84) != 0) {
        unsigned short v;
        fn_0239F9AC();
        v = *(unsigned short *)(off_380FFF4 + 0x300 + 0x84);
        fn_0239A98C(v, 32);
        *(unsigned short *)(off_380FFF4 + 0x300 + 0x84) = 0;
    }
}
