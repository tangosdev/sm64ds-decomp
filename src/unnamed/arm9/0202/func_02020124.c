extern int func_0203da9c(void);
extern unsigned char GetPlayerFlagByte(int player, int byte);
extern void func_0203db3c(int byte, int value);
/* daPly_c 0 only: sets the go flag, bit 5 of link settings byte 2. The
   dScEntry_c sync steps raise it once ArePlayersReady (or
   func_ov075_02116ce4) sees every joined player's ready bit; each
   console then advances when func_02020168 sees it. */
void func_02020124(void) {
    int x;
    if (func_0203da9c() != 0)
        return;
    x = GetPlayerFlagByte(0, 2);
    func_0203db3c(2, (unsigned char)(x | 0x20));
}
