extern int func_0203da9c(void);
extern unsigned char GetPlayerFlagByte(int player, int byte);
extern void func_0203db3c(int byte, int value);

/* daPly_c 0 only: bits 0-3 of link settings byte 2 become flags, one bit
   per player slot. func_02030ca0 and func_02030b58 post the slots
   func_0203d950 reports present; func_0202027c reads a bit back, and
   func_02030b58 moves a joined player whose bit is clear into the
   dropped mask data_0209fc58. */
void func_02020214(unsigned int flags) {
    if (func_0203da9c() != 0)
        return;
    int v = GetPlayerFlagByte(0, 2);
    func_0203db3c(2, (unsigned char)(flags | (v & ~0xf)));
}
