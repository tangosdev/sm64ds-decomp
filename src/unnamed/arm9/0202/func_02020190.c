extern int func_0203da9c(void);
extern unsigned char GetPlayerFlagByte(int player, int byte);
extern void func_0203db3c(int byte, int value);

/* Clears bit 4 of this console's link settings byte 2: the rematch
   ready bit func_020201c0 sets and func_020201f0 tests. */
void func_02020190(void) {
    int v = GetPlayerFlagByte(func_0203da9c(), 2);
    func_0203db3c(2, (unsigned char)(v & ~0x10));
}
