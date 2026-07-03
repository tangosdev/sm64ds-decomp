extern char data_020a7f60[];

void func_0205b504(int idx)
{
    asm {
        ldr r2, =data_020a7f60
        mov r1, #0xc
        mla r1, r0, r1, r2
        ldrb r0, [r1, #8]!
        add r0, r0, #1
        strb r0, [r1]
    }
}