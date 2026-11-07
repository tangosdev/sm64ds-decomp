extern unsigned char GetPlayerFlagByte(int player, int byte);
extern void func_0203db3c(int byte, int value);

/* The writer twin of GetControllerMode: bits 0-1 of link settings byte 4
   (GetPlayerFlagByte reads any player's copy, func_0203db3c writes this
   console's) become mode, the rest of player's byte kept. dScEntry_c::
   UpdateControlSettings calls it with this console's index right after
   SetControllerMode, so the other consoles see the choice. */
void func_02020028(int player, unsigned char mode) {
    int x = GetPlayerFlagByte(player, 4);
    x = mode | (x & ~3);
    func_0203db3c(4, x & 0xff);
}
