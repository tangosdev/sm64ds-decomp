extern int func_0203da9c(void);
extern unsigned char GetPlayerFlagByte(int player, int byte);
extern void func_0203db3c(int byte, int value);

/* Clears bit 3 of this console's link settings byte 0: the ready bit
   func_02020334 sets and func_02020364 tests. */
void func_02020304(void) {
    int v = GetPlayerFlagByte(func_0203da9c(), 0);
    func_0203db3c(0, (unsigned char)(v & ~8));
}
