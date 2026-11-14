extern int func_0203da9c(void);
extern unsigned char GetPlayerFlagByte(int player, int byte);
extern void func_0203db3c(int byte, int value);
/* daPly_c 0 only: clears the go flag, bit 5 of link settings byte 2
   (set by func_02020124, tested by func_02020168). */
void func_020200e0(void) {
    int x;
    if (func_0203da9c() != 0)
        return;
    x = GetPlayerFlagByte(0, 2);
    func_0203db3c(2, (unsigned char)(x & ~0x20));
}
