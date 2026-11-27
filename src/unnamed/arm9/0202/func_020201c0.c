extern int func_0203da9c(void);
extern unsigned char GetLinkByte(int slot, int byte);
extern void SetLinkByte(int byte, int value);

/* Sets bit 4 of this console's link settings byte 2 (dScEntry_c::
   EnterRematchSync, unless a player has dropped). daPly_c 0 polls every
   joined player's copy through IsPlayerReadyForRematch (dScEntry_c::ArePlayersReadyForRematch). */
void func_020201c0(void) {
    int v = GetLinkByte(func_0203da9c(), 2);
    SetLinkByte(2, (unsigned char)(v | 0x10));
}
