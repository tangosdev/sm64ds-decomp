extern int func_0203da9c(void);
extern unsigned char GetPlayerFlagByte(int player, int byte);
extern void func_0203db3c(int byte, int value);

/* Sets bit 3 of this console's link settings byte 0, the ready bit
   dScEntry_c::ArePlayersReady polls on every joined player through
   func_02020364. */
void func_02020334(void) {
    int v = GetPlayerFlagByte(func_0203da9c(), 0);
    func_0203db3c(0, (unsigned char)(v | 8));
}
