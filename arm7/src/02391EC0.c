/* storage 0x02391EC0 runtime 0x027C3E38 size 0x94 main ARM.
 * Clamp the a1 word at +4 into the halfword at s + 0x192, dense-switch it
 * (0 -> 0, 1/2/3 -> 1, default -> 0) into the word at s + 0x194, call
 * fn_02382DC0(a1 + 8 word, s + 0x198, 0x50), then lock, write halfwords
 * {0x14, 0}, unlock. No pool. */

extern char *fn_0238E688(char *, int); /* runtime 0x027C0600 */
extern void fn_02382DC0(int, char *, int); /* runtime 0x037FAC58 */
extern unsigned short *fn_0238DE4C(void); /* runtime 0x03805CE4 */
extern void fn_0238DEF4(void); /* runtime 0x03805D8C */

void fn_02391EC0(char *a1, int a2) {
    char *s = *(char **)*(char **)(fn_0238E688(a1, a2) + 0x1610);
    *(unsigned short *)(s + 0x192) = (unsigned short)*(int *)(a1 + 4);
    switch (*(unsigned short *)(s + 0x192)) {
    case 0:
        *(int *)(s + 0x194) = 0;
        break;
    case 1:
    case 2:
    case 3:
        *(int *)(s + 0x194) = 1;
        break;
    default:
        *(int *)(s + 0x194) = 0;
        break;
    }
    fn_02382DC0(*(int *)(a1 + 8), s + 0x198, 0x50);
    {
        unsigned short *q = fn_0238DE4C();
        q[0] = 0x14;
        q[1] = 0;
        fn_0238DEF4();
    }
}
