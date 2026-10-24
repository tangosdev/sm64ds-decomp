/* storage 0x0238DCEC runtime 0x03805B84 linked 0x70 wram ARM.
 * Reset sixteen channels, clear two bytes and IME, then fn_0238DC7C. */

extern void fn_02383420(void);
extern void fn_02383634(int index, int arg);
extern void fn_02382B4C(int arg);
extern void fn_0238DC7C(void);

void fn_0238DCEC(void) {
    int i;
    fn_02383420();
    for (i = 0; i < 16; i++)
        fn_02383634(i, 1);
    *(volatile unsigned char *)0x04000508 = 0;
    *(volatile unsigned char *)0x04000509 = 0;
    fn_02382B4C(0x10);
    *(volatile unsigned short *)0x04000208 = 0;
    fn_0238DC7C();
}
