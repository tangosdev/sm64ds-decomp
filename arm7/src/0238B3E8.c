/* storage 0x0238B3E8 runtime 0x03803280 size 0x25C (listed 0x244 + pool) wram ARM.
 * Command switch on a1[1] (cases 0x40/0x41/0x42, BEQ-forward dispatch, B to
 * default which falls off the bare epilogue). Case 0x40 runs the
 * lock/sample/unlock sequence with in-pack fn_0238B900/fn_0238BA10 and
 * mirrors the sample to 0x27FFF94 (storing the mirror ADDRESS to
 * 0x27FFF90, via one shared pool slot for both); case 0x41 arms the dispatch
 * table via in-pack fn_0238BD5C (passing in-pack fn_0238B1D4's address) and
 * fn_0238BCF0; case 0x42 uses in-pack fn_0238BD5C(64, 0)/fn_0238BCA4 (nested
 * call, v13 dead in R0). The state halfword block stays a global:
 * each call-separated region hoists it fresh (all R0, never spanning
 * a BL). The 0x42 range check is spelled (unsigned)(w8 - 3) <= 1 with the
 * main path first (SUB/CMP/BHI-over-fail) and w8 kept for the inner == 3
 * (CMP R1, no reload). fn_023883FC takes (int, int); the (unsigned short)
 * casts at this function's variable sites are explicit and emit the
 * LSL#16/LSR#16 pair (a dead switch value rematerializes from [R4,#4]).
 * VOID: the tail paths fall off the bare epilogue (caller 03800104 ignores
 * R0), so there is no R0 live-out; as int, the live-out pushes the two
 * ((int *)word)[8] stores from (R1,R0) to (R2,R1) (verified both ways).
 * Six pools (0x27FFF94/0x27FFF90/0x0400010C/0x0400010E numeric, 0x0380986C
 * data, in-pack 0x0380306C), eleven BLs. IDA audit confirms every BL target
 * and pool word. */

extern int fn_02382AC4(void);
extern int fn_02382AD8(int a1);
extern int fn_023883E0(int a1);
extern int fn_023883C8(int a1);
extern int fn_02388384(int a1);
extern int fn_023883FC(int a1, int a2);
extern int fn_023808C0(int a1);
extern int fn_0238B900(void);
extern unsigned fn_0238BA10(void);
extern unsigned fn_0238BD5C(unsigned a1, int a2);
extern void *fn_0238BCF0(int a1);
extern void *fn_0238BCA4(int a1);
extern int fn_0238B1D4(void);
extern unsigned short word_380986C[];

void fn_0238B3E8(int *a1) {
    switch (a1[1]) {
    case 0x40: {
        int v3 = fn_02382AC4();
        if (fn_023883E0(2) == 0) {
            fn_02382AD8(v3);
            fn_023883FC((unsigned short)a1[1], 4);
            return;
        } else {
            int v4;
            fn_023883C8(2);
            fn_02382AD8(v3);
            if ((a1[2] & 1) == 1) {
                v4 = fn_0238B900();
                if ((a1[2] & 2) != 0)
                    v4 = (unsigned short)(v4 ^ 0x8000);
                *(unsigned short *)0x27FFF94 = (unsigned short)v4;
                *(unsigned *)0x27FFF90 = 0x27FFF94;
            } else {
                int v6;
                v6 = (int)fn_0238BA10();
                if ((a1[2] & 2) != 0)
                    v6 = (unsigned short)(v6 ^ 0x80);
                *(unsigned short *)0x27FFF94 = (unsigned short)v6;
                *(unsigned *)0x27FFF90 = 0x27FFF94;
            }
            fn_023883FC((unsigned short)a1[1], 0);
            fn_02388384(2);
            return;
        }
    }
    case 0x41:
        if (((int *)word_380986C)[8] == 1) {
            int v9;
            word_380986C[29] = 0;
            word_380986C[28] = 0;
            v9 = fn_02382AC4();
            fn_023808C0(64);
            fn_0238BCF0((int)fn_0238BD5C(64, (int)fn_0238B1D4));
            fn_02382AD8(v9);
            *(volatile unsigned short *)0x400010C = word_380986C[26];
            *(volatile unsigned short *)0x400010E = word_380986C[27] | 0xC0;
            fn_023883FC((unsigned short)a1[1], 0);
            ((int *)word_380986C)[8] = 2;
            break;
        } else {
            fn_023883FC((unsigned short)a1[1], 3);
            return;
        }
    case 0x42: {
        int w8 = ((int *)word_380986C)[8];
        if ((unsigned)(w8 - 3) <= 1) {
            int v12;
            *(volatile unsigned short *)0x400010E &= ~0x80;
            v12 = fn_02382AC4();
            fn_0238BCA4((int)fn_0238BD5C(64, 0));
            fn_02382AD8(v12);
            if (((int *)word_380986C)[8] == 3)
                fn_023883FC(0x42, 0);
            else
                fn_023883FC(0x51, 0);
            ((int *)word_380986C)[8] = 0;
            break;
        } else {
            if (w8 == 3)
                fn_023883FC(0x42, 3);
            else
                fn_023883FC(0x51, 3);
            return;
        }
    }
    default:
        break;
    }
}
