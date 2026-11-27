/* storage 0x0238A50C runtime 0x038023A4 size 0x134 wram ARM.
 * Command gate: when a3 is 0, fold a2 bits 8-14 down to a command value
 * v4 and dispatch. Known commands (0, 1, 16-25, 32-41) are acked with
 * (v4, 3) while the busy word is set; otherwise the busy word is raised,
 * the command latched, and the wait helper decides a (v4, 4) ack.
 * Anything else is acked (v4, 1). One 42-entry jumptable, two reloc
 * pool slots (both shared data words). */

extern void fn_0238A4E0(int, int); /* =0x03802378 */
extern int fn_02381600(int *, int, int); /* =0x037F9498 */
extern int unk_3809520[]; /* =0x03809520 */
extern unsigned short unk_3809620[]; /* =0x03809620 */

void fn_0238A50C(int a1, int a2, int a3) {
    if (a3 == 0) {
        unsigned int v4 = ((unsigned int)(a2 & 0x7F00) << 8) >> 16;
        switch (v4) {
        case 0:
        case 1:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
        case 41:
            {
                int *p = unk_3809520;
                if (p[103] != 0) {
                    fn_0238A4E0((int)v4, 3);
                } else {
                    p[103] = 1;
                    unk_3809620[80] = (unsigned short)v4;
                    if (fn_02381600(p, 0, 0) == 0)
                        fn_0238A4E0((int)v4, 4);
                }
            }
            break;
        default:
            fn_0238A4E0((int)v4, 1);
            break;
        }
    }
}
