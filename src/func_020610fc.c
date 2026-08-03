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
