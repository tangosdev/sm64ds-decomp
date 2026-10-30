/* storage 0x02389EAC runtime 0x03801D44 size 0x70 wram ARM.
 * Touch-key init: reads two halfwords, sets low bit via BIC/ORR when bit 7/6
 * set, then tail arg fn(1). Bit tests are bitfield extracts (LSL/LSR pairs);
 * no pool, four BL relocs. */

extern void fn_0238AB54(void *); /* =0x038029EC */
extern void fn_0238AAFC(void *); /* =0x03802994 */
extern void fn_0238AB28(void *); /* =0x038029C0 */
extern int fn_0238AE68(int); /* =0x03802D00 */

typedef struct {
    unsigned short b0:1;
    unsigned short b1:1;
    unsigned short b2:1;
    unsigned short b3:1;
    unsigned short b4:1;
    unsigned short b5:1;
    unsigned short b6:1;
    unsigned short b7:1;
    unsigned short rest:8;
} bits_t;

typedef union {
    unsigned short h;
    bits_t b;
} word_t;

int fn_02389EAC(void) {
    word_t v1;
    word_t v2;
    fn_0238AB54(&v1);
    fn_0238AAFC(&v2);
    if (v1.b.b7 != 0 || v1.b.b6 != 0 || v2.b.b7 != 0) {
        v1.h = (v1.h & ~1u) | 1u;
        fn_0238AB28(&v1);
    }
    return fn_0238AE68(1);
}
