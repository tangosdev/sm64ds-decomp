/* storage 0x023A1C38 runtime 0x027D3BB0 size 0xE4 (listed 0xE0 + pool) main ARM.
 * MATCHED 2004/b56 -O4,s (AH52; AM61 -> AR40 -> AR66 wall broken).
 * Mode chain on *(base + 0x314), base = off_380FFF4 (an int: single deref).
 * Wall keys: (1) result (live-out, r0-homed)
 * anchors the whole allocation — without it everything shifts a register
 * (V24); (2) the D-pair pkt load is a volatile-expression read and D-fail
 * returns memD, so result is dead in the D-block: no store-attractor, pkt
 * emits first, memD reuses dead r0 for `cmp r1, r0`. Plausible as written:
 * these are ARM9-shared sound tables (cf. AH38). */

extern int off_380FFF4;
extern void fn_0239A98C(unsigned int a1, unsigned int a2);
extern int fn_0239B6F0(unsigned short *a1, unsigned int a2);
extern int fn_0239F9AC(int a1);
extern void fn_023969F4(int a1);
extern void fn_02396A98(void);

unsigned int fn_023A1C38(unsigned short *a1) {
    int idx = a1[1];
    int base = off_380FFF4;
    unsigned short *p300 = (unsigned short *)((char *)base + 0x300);
    unsigned int result = p300[10];
    switch (result) {
    case 1: {
        unsigned short *prow = *(unsigned short **)(base + 736);
        result = *(unsigned short *)((char *)prow + idx * 28);
        if (result > 0x20) {
            fn_0239A98C((unsigned short)(idx & 0xFFFF), 0x20);
            fn_0239B6F0(a1 + 15, a1[22]);
            result = (unsigned int)fn_0239F9AC(idx);
        }
        break;
    }
    case 2:
    case 3: {
        unsigned int gate, p15, memD;
        result = (unsigned int)base + 768;
        gate = *(unsigned short *)(result + 16);
        if (gate > 0x20
            && a1[17] == *(unsigned short *)(result + 0x82)
            && a1[16] == *(unsigned short *)(result + 0x80)) {
            p15 = *(volatile unsigned short *)(a1 + 15);
            memD = *(unsigned short *)(result + 0x7E);
            if (p15 != memD)
                return memD;
            fn_023969F4(0x20);
            fn_02396A98();
            result = (unsigned int)fn_0239B6F0(a1 + 15, a1[22]);
        }
        break;
    }
    }
    return result;
}
