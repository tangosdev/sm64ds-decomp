/* __sinit_ov002_021071f4 at 0x021014e4
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
extern struct Dest data_ov002_021097bc;

void __sinit_ov002_021071f4(void) {
    data_ov002_021097bc.p0 = data_ov002_0210aed0;
    data_ov002_021097bc.p1 = data_ov002_0210aee8;
    data_ov002_021097bc.p2 = data_ov002_0210aed8;
    data_ov002_021097bc.p3 = data_ov002_0210aef8;
    data_ov002_021097bc.p4 = data_ov002_0210aef0;
    data_ov002_021097bc.p5 = data_ov002_0210aee0;
}
