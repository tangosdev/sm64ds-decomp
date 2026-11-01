// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x023990E4 size 0x1F4 (runtime 0x027CB05C, main)
// C tried by: AM50, AR44; asm taken from the AR44 branch.
/* storage 0x023990E4 runtime 0x027CB05C size 0x1B4 + pool 0x1F4 main ARM.
 * Status/counter sync: two guarded counter paths, then a range check
 * against two scaled bounds that snapshots state and queues followup.
 * Whole-asm enrollment: the taken-path zero homes in dead R4
 * (mov r4, #0) while every C shape under b56 -O4,s puts it in scratch
 * ip (mov ip, #0), shifting 8 words. 19 shapes tried (6 AM50 + 13 here:
 * unsigned-compare baseline, zero/var/literal, double/triple duty,
 * no-result, q-reuse, SUB-self, AND, early-init, decl order, goto
 * layout, shift, address-taken, copy-diff); SUB/AND/shift all fold to
 * mov ip, #0. Compiler wraps the block with push/pop/bx; pools emit in
 * first-use order at function end. */
extern int dword_380FFF4[];
extern int fn_0238D594(unsigned int, int);
extern int fn_02395008(void);
int fn_023990E4(void) {
    asm {
        mov r1, #4
        ldr r0, =0x4808010
        strh r1, [r0]
        ldr r0, =0x48081A8
        ldrh r1, [r0]
        ldr r0, =dword_380FFF4
        ldr r0, [r0]
        add r2, r0, #0x600
        ldrh r2, [r2, #0x1c]
        ands r2, r2, #8
        beq L170
        ands r2, r1, #0x400
        beq L170
        add r4, r0, #0x3e4
        ldr r2, =0x48080B0
        ldrh r3, [r2]
        ands r2, r3, #1
        beq L0B4
        ldrh r2, [r4]
        cmp r2, #0
        bne L114
    L0B4:
        ands r2, r3, #4
        beq L0C8
        ldrh r2, [r4, #0x14]
        cmp r2, #0
        bne L114
    L0C8:
        ands r2, r3, #8
        beq L0DC
        ldrh r2, [r4, #0x28]
        cmp r2, #0
        bne L114
    L0DC:
        ldr r2, =0x480819C
        ldrh r2, [r2]
        ands r2, r2, #1
        bne L114
        mov r3, #0
        ldr r2, =0x4808032
        strh r3, [r2]
        mov r0, #0x8000
        strh r0, [r2]
        ldr r0, =dword_380FFF4
        ldr r0, [r0]
        add r0, r0, #0x400
        strh r3, [r0, #0x9a]
        b L170
    L114:
        add r2, r0, #0x400
        ldrh r3, [r2, #0x9a]
        ldr r2, =0x49a
        add r2, r0, r2
        ldrh r0, [r2]
        add r0, r0, #1
        strh r0, [r2]
        cmp r3, #0xc
        bls L170
        mov r4, #0
        ldr r3, =dword_380FFF4
        ldr r0, [r3]
        add r0, r0, #0x400
        strh r4, [r0, #0x9a]
        ldr r2, =0x4808032
        strh r4, [r2]
        mov r0, #0x8000
        strh r0, [r2]
        ldr r2, [r3]
        ldr r0, =0x3b6
        ldrh r0, [r2, r0]!
        add r0, r0, #1
        strh r0, [r2]
    L170:
        ldr r0, =dword_380FFF4
        ldr r0, [r0]
        add r0, r0, #0x600
        ldrh r0, [r0, #0x1c]
        ands r0, r0, #1
        beq L208
        ands r0, r1, #0x60
        beq L208
        ldr r0, =0x4808054
        ldrh r4, [r0]
        ldr r0, =0x4808052
        ldrh r0, [r0]
        sub r0, r0, #0x4000
        mov r1, #2
        bl fn_0238D594
        cmp r4, r0
        bge L1D0
        ldr r0, =0x4808050
        ldrh r0, [r0]
        sub r0, r0, #0x4000
        mov r1, #2
        bl fn_0238D594
        cmp r4, r0
        bge L204
    L1D0:
        ldr r0, =0x480805a
        ldrh r1, [r0]
        ldr r0, =0x4808056
        strh r1, [r0]
        ldr r1, =0x8001
        ldr r0, =0x4808030
        strh r1, [r0]
        ldr r0, =dword_380FFF4
        ldr r1, [r0]
        ldr r0, =0x496
        ldrh r0, [r1, r0]!
        add r0, r0, #1
        strh r0, [r1]
    L204:
        bl fn_02395008
    L208:
    }
}
