/* storage 0x023969F4 runtime 0x027C896C size 0xA4 main.
 * Mode setter for the WRAM state block: dispatches 0/16/64 transitions,
 * stores the new mode. Two pool refs (off_380FFF4, fn_023955A4), six calls. */

extern unsigned char *off_380FFF4; /* =0x0380FFF4 */
extern void fn_023954C4(void); /* =0x027C743C */
extern void fn_02396E6C(void); /* =0x027C8DE4 */
extern void fn_02395CB8(void); /* =0x027C7C30 */
extern void fn_02396DD8(void); /* =0x027C8D50 */
extern void fn_02396CC0(void); /* =0x027C8C38 */
extern void fn_023955A4(void); /* =0x027C751C */
extern void fn_0239565C(int a1, int a2); /* =0x027C75D4 */

void fn_023969F4(int a1) {
    unsigned short cur = *(unsigned short *)(off_380FFF4 + 0x300 + 0x10);
    if (cur == a1)
        return;
    if (cur == 64)
        fn_023954C4();
    switch (a1) {
    case 0:
        fn_02396E6C();
        break;
    case 16:
        fn_02395CB8();
        fn_02396DD8();
        break;
    case 64:
        if (*(unsigned short *)(off_380FFF4 + 0x300 + 0x14) == 2)
            fn_02396CC0();
        fn_0239565C(100, (int)fn_023955A4);
    }
    *(unsigned short *)(off_380FFF4 + 0x300 + 0x10) = a1;
}
