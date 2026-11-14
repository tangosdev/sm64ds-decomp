/* storage 0x0238B644 runtime 0x038034DC size 0x27C (listed 0x274 + pool) wram ARM.
 * Command dispatch on the high byte of base[0] (v5 = ((v4 & 0xFF00) << 8) >> 16,
 * unsigned shifts: AND/LSL#8/LSR#16). Cases 64/65/66 set up fn_023882B0 /
 * fn_023883FC calls; case 65 validates a (base,len) window and derives tier
 * fields [27] = 0..3, [26] = 0x10000 - (v9 >> shift). The state base stays a
 * global (one hoisted R1 for the call-free first region; per-statement
 * reloads after calls, including the predicated LDRNE/STRNE pair).
 * Halfwords are unsigned (LDRH, never LDRSH). v1 doubles as the zero-loop
 * counter and later v5 (one var, R4); z is copy-initialized from v1, which
 * is what pins MOV R3,R4 ahead of the pool load. Two pools (0x0380986C
 * data, 0x400010E numeric), BLs only to 0x03800148/0x03800294. IDA audit
 * confirms every BL target and pool word. */

extern int fn_023882B0(int a1, int a2, int a3, ...);
extern int fn_023883FC(int a1, int a2);
extern unsigned short word_380986C[];

void fn_0238B644(unsigned a1) {
    int v1;
    unsigned short v4;
    unsigned v5;
    if ((a1 & 0x2000000) != 0) {
        int z;
        v1 = 0;
        z = v1;
        do
            word_380986C[v1] = z;
        while (++v1 < 16);
    }
    word_380986C[(a1 & 0xF0000) >> 16] = (unsigned short)a1;
    if ((a1 & 0x1000000) == 0)
        return;
    v4 = word_380986C[0];
    v5 = ((unsigned)(v4 & 0xFF00) << 8) >> 16;
    switch (v5) {
    case 64:
        if (fn_023882B0(2, v5, 1, v4 & 0xFF) != 0)
            return;
        fn_023883FC(v5, 4);
        return;
        break;
    case 65:
        if (((int *)word_380986C)[8] != 0) {
            fn_023883FC(v5, 3);
            return;
        }
        word_380986C[18] = v4 & 0xFF;
        {
            unsigned v7 = ((unsigned)word_380986C[1] << 16) | (unsigned)word_380986C[2];
            unsigned v8;
            unsigned v9;
            if (v7 < 0x2000000 || v7 >= 0x2400000) {
                fn_023883FC(v5, 2);
                return;
            }
            ((int *)word_380986C)[10] = (int)v7;
            v8 = ((unsigned)word_380986C[3] << 16) | (unsigned)word_380986C[4];
            if (v7 + v8 > 0x2400000) {
                fn_023883FC(v5, 2);
                return;
            }
            ((int *)word_380986C)[12] = (int)v8;
            v9 = ((unsigned)word_380986C[5] << 16) | (unsigned)word_380986C[6];
            if (v9 < 0x10000) {
                word_380986C[27] = 0;
                word_380986C[26] = (unsigned short)(0x10000 - v9);
            } else if (v9 < 0x400000) {
                word_380986C[27] = 1;
                word_380986C[26] = (unsigned short)(0x10000 - (v9 >> 6));
            } else if (v9 < 0x1000000) {
                word_380986C[27] = 2;
                word_380986C[26] = (unsigned short)(0x10000 - (v9 >> 8));
            } else if (v9 < 0x4000000) {
                word_380986C[27] = 3;
                word_380986C[26] = (unsigned short)(0x10000 - (v9 >> 10));
            } else {
                fn_023883FC(v5, 2);
                return;
            }
            ((int *)word_380986C)[11] = 0;
            word_380986C[19] = word_380986C[18] & 3;
            if (fn_023882B0(2, v5, 0) != 0) {
                ((int *)word_380986C)[8] = 1;
                return;
            }
            fn_023883FC(v5, 4);
            return;
        }
        break;
    case 66:
        if (((int *)word_380986C)[8] != 2) {
            fn_023883FC(v5, 3);
            return;
        }
        if (fn_023882B0(2, v5, 0) == 0) {
            fn_023883FC(v5, 4);
            return;
        }
        ((int *)word_380986C)[8] = 3;
        *(volatile unsigned short *)0x400010E &= ~0x80;
        return;
    default:
        fn_023883FC(v5, 1);
    }
}
