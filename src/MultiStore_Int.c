asm void MultiStore_Int(int val, int *dst, int len) {
    add ip, r1, r2
loop:
    cmp r1, ip
    stmltia r1!, {r0}
    blt loop
    bx lr
}
