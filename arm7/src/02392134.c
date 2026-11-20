/* storage 0x02392134 runtime 0x027C40AC size 0xBC (code 0xB8 + 1-word pool) main ARM.
 * Dispatch helper: 5-arg request via 0x027C1A04 (stor 0238FA8C) on a
 * 0x200-byte stack buffer (mode 10/20 from arg+6 == 1, 5th arg on stack),
 * then on the reply short at res[2]: nonzero sends (0x309, res[2]) through
 * 0x027C4070 (stor 023920F8, pool word 0x309); zero forces the shared
 * short to 4 unless already 4, retries via 0x027C1D34 (stor 0238FDBC),
 * sends (0x204, res[2]) on nonzero, else a plain 0x1A message. The zero
 * path shares one plain-message tail: q==4 jumps into the else (goto
 * plain), which is also where the failed retry falls through. */

extern void *fn_0238E688(void); /* =0x027C0600 */
extern unsigned short *fn_0238FA8C(unsigned short *a1, unsigned short a2, unsigned short a3, int a4, int a5); /* =0x027C1A04 */
extern unsigned short *fn_0238FDBC(unsigned short *a1, short a2); /* =0x027C1D34 */
extern int fn_023920F8(unsigned short a1, unsigned short a2); /* =0x027C4070 */
extern unsigned short *fn_0238DE4C(void); /* =0x03805CE4 */
extern int fn_0238DEF4(unsigned short *msg); /* =0x03805D8C */

typedef struct {
    int pad00;
    unsigned short s04;
    unsigned short s06;
    unsigned short s08;
} arg_02392134_t;

void fn_02392134(arg_02392134_t *a) {
    unsigned short buf[256];
    char *base = (char *)fn_0238E688();
    char *hi = base + 0x1000;
    int *outer = *(int **)(hi + 0x610);
    unsigned short *r = fn_0238FA8C(buf, 1, a->s04, (a->s06 == 1) ? 10 : 20, a->s08);
    if (r[2] != 0) {
        fn_023920F8(0x309, r[2]);
    } else {
        unsigned short *q = (unsigned short *)*outer;
        if (*q == 4)
            goto plain;
        *q = 4;
        r = fn_0238FDBC(buf, 0);
        if (r[2] != 0)
            fn_023920F8(0x204, r[2]);
        else {
plain:
            unsigned short *msg = fn_0238DE4C();
            msg[0] = 0x1A;
            msg[1] = 0;
            fn_0238DEF4(msg);
        }
    }
}
