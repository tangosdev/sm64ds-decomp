/* __sinit_ov002_021071f4 at 0x021071f4
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (overlay ov002).
 */

typedef struct { int a, b; } Pair;

extern Pair data_ov002_0210aed0;
extern Pair data_ov002_0210aee8;
extern Pair data_ov002_0210aed8;
extern Pair data_ov002_0210aef8;
extern Pair data_ov002_0210aef0;
extern Pair data_ov002_0210aee0;

struct Dest {
    Pair p0;   // 0x00
    Pair p1;   // 0x08
    int  gap0; // 0x10
    Pair p2;   // 0x14
    Pair p3;   // 0x1c
    int  gap1; // 0x24
    Pair p4;   // 0x28
    Pair p5;   // 0x30
};
extern struct Dest data_ov002_0210af2c;

/* The destination is data_ov002_0210af2c, not data_ov002_021097bc. The ROM
   materialises the base into r5 from the literal pool and stores through
   [r5, #off]; those stores carry no relocation, so only the single pool word
   differed -- and match.py wildcards relocated words, so the per-function gate
   passed this happily. Only the full ROM link sees it, and it could not see it
   before either, because .init functions were not enrolled at all. */
void __sinit_ov002_021071f4(void) {
    data_ov002_0210af2c.p0 = data_ov002_0210aed0;
    data_ov002_0210af2c.p1 = data_ov002_0210aee8;
    data_ov002_0210af2c.p2 = data_ov002_0210aed8;
    data_ov002_0210af2c.p3 = data_ov002_0210aef8;
    data_ov002_0210af2c.p4 = data_ov002_0210aef0;
    data_ov002_0210af2c.p5 = data_ov002_0210aee0;
}
