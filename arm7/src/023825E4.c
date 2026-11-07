/* storage 0x023825E4 runtime 0x037FA47C size 0x140 (listed 0x130 + pool) wram ARM.
 * Scheduler pump: mask a display bit, flag shared work, then drain the
 * unk_3806FF0 node list: unlink each head node, run its callback (word 0, if
 * nonzero) with word 1 as argument, mark it done, re-queue it via fn_02382874
 * when word 6 is set, and stop at the first node whose key window still holds.
 * Afterwards refresh the next node via fn_02382A0C and clear every done mark.
 * Four pools (three numeric consts, one address), four calls plus the indirect
 * callback (MOV LR,PC + BX R7). */

extern int fn_02380884(int a1);
extern int unk_3806FF0[];
extern int fn_02382874(int *a1);
extern int fn_02382A80(int a1);
extern int fn_02382A0C(int *a1);

int fn_023825E4(void) {
    volatile unsigned short *v0;
    int result;
    int *v2;
    int *v3;
    int v4;
    int v5;
    int *v6;
    int v7;
    int *v8;
    fn_02380884(4);
    *(volatile unsigned short *)0x4000004 &= ~0x20u;
    *(volatile int *)0x380FFF8 |= 4;
    v0 = (volatile unsigned short *)0x4000006;
    result = *v0;
    v2 = unk_3806FF0;
    v3 = (int *)*v2;
    if (v3 != 0) {
        while (1) {
            v4 = v3[5];
            *v2 = v4;
            if (v4 == 0)
                v2[1] = 0;
            else
                ((int *)v4)[4] = 0;
            v5 = v3[0];
            v3[0] = 0;
            if (v5 != 0)
                ((void (*)(int))v5)(v3[1]);
            v3[7] = 1;
            if (v3[6] != 0) {
                v3[0] = v5;
                fn_02382874(v3);
            }
            v6 = (int *)*v2;
            if (v6 == 0)
                break;
            if (v6[7] != 0)
                break;
            v7 = fn_02382A80((int)*v0 - (int)((short *)v6)[6]);
            v6 = (int *)*v2;
            if (v7 < (int)((short *)v6)[7]
                && fn_02382A80((int)((short *)v6)[6] - (int)((short *)v3)[6])
                    <= (int)((short *)v6)[7]) {
                v3 = v6;
                continue;
            }
            break;
        }
        if (v6 != 0)
            fn_02382A0C(v6);
        v8 = (int *)unk_3806FF0[0];
        result = 0;
        while (v8 != 0) {
            v8[7] = 0;
            v8 = (int *)v8[5];
        }
    }
    return result;
}
