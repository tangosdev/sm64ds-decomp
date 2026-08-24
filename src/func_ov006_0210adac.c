/* func_ov006_0210adac @ 0x0210adac (ov006, size 0x1b8)   [mwccarm 2004/b56]
 * dScMgSlot3_c (Mix-a-Mug, actor 0x16d / scene 365) state 2: the face
 * evaluation between "the third reel has stopped" and the payout. It is slot 2
 * of the class's state table data_ov006_02142bdc, entered from vtable slot 6
 * (func_ov006_0210bcb0) and it is the step that decides what the round pays.
 *
 * c + 0x503e   the shared hold-down counter. DecIfAbove0_Byte decrements it and
 *              returns nonzero while it is still running, so the body is a
 *              no-op until the reels have finished settling.
 * c + 0x5031   the three faces reel 0 is showing, one per row, read into buf[].
 * c + 0x4fe4   the three reel angles, one int per reel; (angle >> 12) / 80 is
 *              how many rows that reel has turned past its origin.
 * c + 0x503a   the number of faces on a reel (the modulus).
 * c + 0x501c   the face grid, one five-byte strip per reel at + 5 * reel.
 * c + 0x503b   the face id that pays.
 * c + 0x5010   the winning row index, or -1.
 * c + 0x5014   the payout.
 * c + 0x5000   the state index: 3 on a win, 4 on a loss. Both table slots hold
 *              func_ov006_0210ac3c, which tells the two apart by the index it
 *              was entered on.
 *
 * So row i pays when reels 1 and 2 both show reel 0's face at that row. A row
 * that disagrees on either has its face replaced with the sentinel 5, which can
 * only pay if the paying face at c + 0x503b is itself 5.
 *
 * THE `(int)` ON `i` IS LOAD-BEARING. It is a no-op on an `int`, but it changes
 * the shape mwccarm builds for the sum, and with it the strength-reduced row
 * base `c + 5 * j` keeps its initialiser inside the outer loop (`add r6,sl,#5`)
 * instead of being hoisted into the prologue and spilled to a stack slot. Take
 * the cast out and the function grows two instructions and an 8-byte frame and
 * stops matching; `(long)i` does the same job. See config/match_provenance.jsonl.
 */
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern void func_02012790(int arg);
extern void func_ov004_020adb1c(int self);

typedef struct SlotGrid {
    unsigned char pad[0x501c];
    unsigned char sym[3][5];
} SlotGrid;

void func_ov006_0210adac(char *c) {
    unsigned char buf[3];
    int i;
    int total;
    int arg;

    if (DecIfAbove0_Byte((unsigned char *)(c + 0x503e)) != 0) return;

    for (i = 0; i < 3; i++) {
        int j;
        buf[i] = *(unsigned char *)(c + 0x5031 + i);
        j = 1;
        do {
            int rem = ((int)i + (((int *)(c + 0x4fe4))[j] >> 12) / 80) % *(unsigned char *)(c + 0x503a);
            if (buf[i] != ((SlotGrid *)c)->sym[j][rem]) {
                buf[i] = 5;
                break;
            }
        } while (++j < 3);
    }

    *(int *)(c + 0x5010) = -1;
    total = 0;
    {
        int k;
        for (k = 0; k < 3; k++) {
            if (buf[k] == *(unsigned char *)(c + 0x503b)) {
                if (k == 1) {
                    *(int *)(c + 0x5010) = k;
                    total += 6;
                } else if (k == 0 || k == 2) {
                    *(int *)(c + 0x5010) = k;
                    total += 3;
                }
                break;
            }
        }
    }

    arg = *(int *)(c + 0xb4);
    if (total > 0) {
        *(int *)(c + 0x5014) = total;
        *(int *)(c + 0x5000) = 3;
        func_02012790(0x26);
        *(unsigned char *)(c + 0x503e) = 0x28;
        arg += 1;
    } else {
        *(int *)(c + 0x5000) = 4;
        func_02012790(0xe);
        *(unsigned char *)(c + 0x503e) = 0x50;
    }
    func_ov004_020adb1c(arg);

    *(unsigned char *)(c + 0xc3) = 0;
}
