/* storage 0x02386970 runtime 0x037FE808 size 0x58 + one pool word, wram ARM.
 * Slot setup. a5 is a stack argument used once after the call; without
 * opt_propagation off, b56 reloads it and saves only r4-r7. The pragma
 * homes a5 in r5 in the prologue (push {r4-r8,lr}, str r5). */

extern unsigned char byte_3808544[];
extern void fn_0238219C(void *);

#pragma opt_propagation off

int fn_02386970(int a1, int a2, int a3, int a4, int a5, int a6) {
    int s5 = a5;
    unsigned char *row = byte_3808544 + (a1 << 6);
    if (byte_3808544[a1 << 6] != 0) {
        fn_0238219C(row + 0x14);
        *row = 0;
    }
    *(int *)(row + 4) = a2;
    *(int *)(row + 8) = a3;
    *(int *)(row + 12) = a4;
    *(int *)(row + 16) = s5;
    row[1] = (unsigned char)a6;
    return a6;
}
