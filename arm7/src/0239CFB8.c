/* storage 0x0239CFB8 runtime 0x027CEF30 size 0x164 main. */

extern unsigned char *off_380FFF4; /* =0x0380FFF4 */
extern int fn_02395DA8(void); /* =0x027C7D20 */
extern void fn_02395CB8(int); /* =0x027C7C30 */
extern int fn_02395C74(int); /* =0x027C7BEC */
extern void fn_02395BE0(int, int); /* =0x027C7B58 */
extern void fn_02382C80(int, void *, int); /* =0x037FAB18 */
extern void fn_023970C8(int, int); /* =0x027C9040 */

void fn_0239CFB8(unsigned short *a1) {
    volatile unsigned short s0;
    unsigned short *mm;
    unsigned int v5;
    unsigned short *v4;
    int v2 = fn_02395DA8();
    fn_02395CB8(v2);
    *(unsigned short *)(off_380FFF4 + 0x300 + 0xA8) = fn_02395C74(6);
    if (a1[9] == 4)
        fn_02395BE0(6, 0);
    s0 = 0;
    fn_02382C80(s0, (void *)0x4804000, 12);
    mm = (unsigned short *)0x4804000;
    mm[4] = 20;
    mm[5] = 2000;
    v4 = mm + 6;
    v5 = 0;
    do {
        *v4++ = 0x5555;
        v5 += 2;
    } while (v5 < 0x5F8);
    mm[6] = 8;
    *(unsigned short *)0x4808194 = 6;
    *(unsigned short *)(off_380FFF4 + 0x300 + 0x10) = 18;
    *(unsigned short *)(off_380FFF4 + 0x300 + 0x16) |= 0x20;
    *(unsigned short *)0x4808040 = 0x8000;
    fn_023970C8(a1[11], 1);
    *(unsigned short *)0x4808012 = 2;
    *(unsigned short *)0x4808004 = 1;
    *(unsigned short *)0x48080AE = 1;
    *(unsigned short *)0x48080A0 = (unsigned short)(((unsigned int)mm & 0x3FFF) >> 1) | 0x8000;
}
