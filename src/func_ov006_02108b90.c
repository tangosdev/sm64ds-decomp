// NONMATCHING: different op / idiom. The compiler folds the ROM's cmp/branch
// pairs into conditional moves, so the candidate is 0x104 against the ROM's
// 0x130 and the byte gate cannot score this body at all
// (see notes/matching-style.md). Counts as decompiled, not matched.
//
// THE "LOGIC VERIFIED CORRECT VS ROM" LINE THAT USED TO STAND HERE WAS WRONG,
// and it was the whole of what stood behind the arithmetic of Mushroom
// Roulette's payout. The 0x23 arm -- the top half of the board, one of the two
// colour bets -- read `r1 >= 6` where the ROM's blt at 0x02108c84 pays on
// `r1 < 6`. Inverted: a winning bet on that half paid nothing and a losing one
// paid 2. Because this file is NONMATCHING there was no gate to catch it, so
// port/hal/scene_mg_roulette.cpp now sweeps all 38 bet types x 12 pockets
// against the rule transcribed from the ROM and prints the disagreement count.
// It reported 12 before the fix and 0 after.
//
// The two arms are NOT symmetric and the ROM says so twice over:
//   0x02108c78  cmp r3,#0x23 / bne / cmp r1,#6 / blt -> mov r2,#2   pocket <  6
//   0x02108c88  cmp r3,#0x24 / bne / cmp r1,#6 / blt -> skip        pocket >= 6
// which is also what the board geometry says: func_ov006_02108650 puts 0x23 in
// the strip cell over y 0x10..0x60 and 0x24 in the one over y 0x60..0xb0, and
// the twelve pockets run 0..5 across the top three rows and 6..11 across the
// bottom three.
extern short data_ov006_0212edb8[];
extern short data_ov006_0212edba[];
extern short data_ov006_0212ed88[];
extern short data_ov006_0212ed8a[];
extern short data_ov006_0212ed8c[];
extern short data_ov006_0212ed8e[];

int func_ov006_02108b90(short *r0, int r1)
{
    int r3 = *(short *)((char *)r0 + 0x2c);
    int r2 = 0;
    int ip;
    ip = (r3 <= 0xb) ? 1 : 0;
    if (ip != 0) {
        if (r3 == r1) r2 = 0xc;
    } else if (r3 >= 0xc) {
        ip = (r3 <= 0x1c) ? 1 : 0;
        if (ip != 0) {
            ip = (r3 - 0xc) << 2;
            if (r1 == *(short *)((char *)data_ov006_0212edb8 + ip) ||
                r1 == *(short *)((char *)data_ov006_0212edba + ip))
                r2 = 6;
        } else if (r3 >= 0x1d) {
            ip = (r3 <= 0x22) ? 1 : 0;
            if (ip != 0) {
                ip = (r3 - 0x1d) << 3;
                if (r1 == *(short *)((char *)data_ov006_0212ed88 + ip) ||
                    r1 == *(short *)((char *)data_ov006_0212ed8a + ip) ||
                    r1 == *(short *)((char *)data_ov006_0212ed8c + ip) ||
                    r1 == *(short *)((char *)data_ov006_0212ed8e + ip))
                    r2 = 3;
            } else if (r3 == 0x23) {
                if (r1 < 6) r2 = 2;
            } else if (r3 == 0x24) {
                if (r1 >= 6) r2 = 2;
            }
        }
    }
    *(short *)((char *)r0 + 0x2e) = r2;
    return r2;
}
