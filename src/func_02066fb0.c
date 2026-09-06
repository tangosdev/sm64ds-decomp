/* func_02066fb0 at 0x02066fb0
 *
 * The hook pointer lives at +0x14e4 of the block; it is reached as +0x1000
 * then +0x4e4 (add r3,r3,#0x1000 / ldr r3,[r3,#0x4e4]). Spelled as one
 * +0x14e4 mwcc materializes the constant from the literal pool and indexes
 * with a register, one word longer than the cartridge.
 */
extern char *data_020a9db8;

int func_02066fb0(int a, int b, int c) {
    char *base = data_020a9db8 + 0x1000;
    int (*fn)(int, int, int) = *(int (**)(int, int, int))(base + 0x4e4);
    if (fn) return fn(a, b, c);
}
