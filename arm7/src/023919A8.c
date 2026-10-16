/* storage 0x023919A8 runtime 0x027C3920 size 0x58 main ARM.
 * Branch on fn_02391698(a1, 0) == 1: lock, write halfwords {13, 0 or 1},
 * unlock. Both arms duplicate the lock/write/unlock sequence. No pool. */

extern int fn_02391698(int, int); /* runtime 0x027C3610 */
extern unsigned short *fn_0238DE4C(void); /* runtime 0x03805CE4 */
extern void fn_0238DEF4(void); /* runtime 0x03805D8C */

void fn_023919A8(int a1) {
    if (fn_02391698(a1, 0) == 1) {
        unsigned short *p = fn_0238DE4C();
        p[0] = 13;
        p[1] = 0;
        fn_0238DEF4();
    } else {
        unsigned short *p = fn_0238DE4C();
        p[0] = 13;
        p[1] = 1;
        fn_0238DEF4();
    }
}
