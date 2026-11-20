/* storage 0x02391C98 runtime 0x027C3C10 size 0x74 main ARM.
 * Resolve a double-indirect table base via fn_0238E688, clear the word at
 * +0x10, fold halfword 10->8 / 9->7, run fn_02382724 on the base + 0xD0,
 * then lock, write halfwords {0x10, 0}, unlock. No pool. */

extern char *fn_0238E688(int, int); /* runtime 0x027C0600 */
extern void fn_02382724(char *); /* runtime 0x037FA5BC */
extern unsigned short *fn_0238DE4C(void); /* runtime 0x03805CE4 */
extern void fn_0238DEF4(void); /* runtime 0x03805D8C */

void fn_02391C98(int a1, int a2) {
    char *s = fn_0238E688(a1, a2);
    char *p2 = *(char **)*(char **)(s + 0x1610);
    *(int *)(p2 + 0x10) = 0;
    if (*(unsigned short *)p2 == 10)
        *(unsigned short *)p2 = 8;
    else if (*(unsigned short *)p2 == 9)
        *(unsigned short *)p2 = 7;
    fn_02382724(*(char **)(s + 0x1610) + 0xD0);
    {
        unsigned short *q = fn_0238DE4C();
        q[0] = 0x10;
        q[1] = 0;
        fn_0238DEF4();
    }
}
