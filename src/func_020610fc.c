// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// HAND-ASM: IPC send loop (cmd 0xd, arg 2) followed by an intentional hang
// (b self). The symbol's span also contains an unreachable epilogue-less tail
// that stores r0 into data_020a89a4 - not producible from a single C function,
// so this is kept as asm.
extern void IPCSend(void);
extern void data_020a89a4(void);

asm void func_020610fc(void)
{
    stmdb sp!, {r4, r5, r6, lr};
    mov r6, #0xd;
    mov r5, #2;
    mov r4, #0;
loop:
    mov r0, r6;
    mov r1, r5;
    mov r2, r4;
    bl IPCSend;
    cmp r0, #0;
    bne loop;
done:
    b done;
    ldr r1, [pc, #4];
    str r0, [r1];
    bx lr;
    dcd data_020a89a4;
}
