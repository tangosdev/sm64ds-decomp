// NONMATCHING: different op / idiom (div=5, was 24 when the note was written
// and 22 as measured now). Not byte-matchable from C at mwccarm 1.2/sp2p3
// (see notes/matching-style.md). Counts as decompiled, not matched.
//
// THE CALL AT THE BOTTOM USED TO DROP BOTH ARGUMENTS. func_ov006_02108650 is
// the board classifier and takes (x, y); this file declared it `(void)` and
// called it with nothing. On ARM that was invisible -- the ROM's own
// 0x02108dc8 `bl 0x2108650` reaches it with r0 and r1 already holding the two
// stylus bytes loaded at 0x02108da0 and 0x02108dac, and the candidate happened
// to leave one of them live -- but on the host the callee read two stack slots
// the caller never wrote, so the "is the stylus over a board cell" test
// answered from garbage and this grab path never fired. That is the TAP-TO-
// PLACE half of the input: func_ov006_02108e24 (drag a mushroom that is
// already under the stylus) was unaffected, so the game was still playable by
// dragging and the loss was silent. Spelling the two arguments takes the
// remaining divergence from 22 words to 5, and all five are ip/lr register
// transpositions of the same instruction.
struct Quad { unsigned char b0, b1, b2, b3; };
extern struct Quad data_020a0de8[];
extern unsigned char data_020a0e40[];
extern void *data_ov006_021428c8;
extern int func_ov006_02108650(int x, int y);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

struct Obj {
    int x0;
    int x4;
    int x8;
    int xc;
    char pad10[0x1c];
    short x2c;
    char pad2e[4];
    unsigned char x32;
};

void func_ov006_02108d28(struct Obj *o)
{
    int flag;
    int dx, dy;
    int idx;

    if (data_ov006_021428c8 != 0) return;

    idx = data_020a0e40[0];
    flag = 0;
    if (data_020a0de8[idx].b0 != 0) {
        if (data_020a0de8[idx].b1 != 0) flag = 1;
    }
    if (flag == 0) return;

    if (o->x32 != 1) return;

    dx = (o->x0 >> 12) - data_020a0de8[idx].b2;
    dy = (o->x4 >> 12) - data_020a0de8[idx].b3;
    if (o->x2c != 0x25) return;

    if (func_ov006_02108650(data_020a0de8[idx].b2, data_020a0de8[idx].b3) == 0x25) return;

    _ZN5Sound12PlayBank2_2DEj(0x15d);
    data_ov006_021428c8 = o;
    o->x32 = 2;
    o->x8 = dx << 12;
    o->xc = dy << 12;
}
