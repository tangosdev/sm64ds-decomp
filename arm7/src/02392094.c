/* storage 0x02392094 runtime 0x027C400C size 0x60 (code 0x5C + 1-word pool) main ARM.
 * Query helper: builds a 0x19 request via 0x027C1C08 (stor 0238FC90) on a
 * 0x200-byte stack buffer, passing (unsigned short)arg[1] (lsl/lsr pair),
 * then branches on the reply short at res[2]: nonzero sends (0x215, res[2])
 * through 0x027C3FD0 (stor 02392058, pool word 0x215), zero allocs/sends a
 * plain 0x19 message via 0x03805CE4/0x03805D8C. */

extern void *fn_0238E688(void); /* =0x027C0600 */
extern unsigned short *fn_0238FC90(unsigned short *a1, unsigned short a2); /* =0x027C1C08 */
extern int fn_02392058(int a1, int a2); /* =0x027C3FD0 */
extern unsigned short *fn_0238DE4C(void); /* =0x03805CE4 */
extern int fn_0238DEF4(unsigned short *msg); /* =0x03805D8C */

typedef struct {
    int pad00;
    int v04;
} arg_02392094_t;

void fn_02392094(arg_02392094_t *a) {
    unsigned short buf[256];
    unsigned short *res;
    fn_0238E688();
    res = fn_0238FC90(buf, a->v04);
    if (res[2] != 0) {
        fn_02392058(0x215, res[2]);
    } else {
        unsigned short *msg = fn_0238DE4C();
        msg[0] = 0x19;
        msg[1] = 0;
        fn_0238DEF4(msg);
    }
}
