extern int func_0203da9c(void);
extern unsigned char GetLinkByte(int slot, int byte);
extern void SetLinkByte(int byte, int value);

/* Sets bit 3 of this console's link settings byte 0, the ready bit
   dScEntry_c::ArePlayersReady polls on every joined player through
   IsPlayerReady. */
void func_02020334(void) {
    int v = GetLinkByte(func_0203da9c(), 0);
    SetLinkByte(0, (unsigned char)(v | 8));
}
