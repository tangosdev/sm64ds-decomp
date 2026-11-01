extern int func_0203da9c(void);
extern unsigned char GetPlayerFlagByte(int player, int byte);
extern void func_0203db3c(int byte, int value);

/* Sets bit 4 of this console's link settings byte 2 (dScEntry_c::
   EnterRematchSync, unless a player has dropped). daPly_c 0 polls every
   joined player's copy through func_020201f0 (func_ov075_02116ce4). */
void func_020201c0(void) {
    int v = GetPlayerFlagByte(func_0203da9c(), 2);
    func_0203db3c(2, (unsigned char)(v | 0x10));
}
