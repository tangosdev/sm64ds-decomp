extern unsigned char _Z17GetPlayerFlagByteii(int player, int byte);
extern void _Z13func_0203db3cii(int byte, int value);

/* The writer twin of GetControllerMode: bits 0-1 of link settings byte 4
   (GetPlayerFlagByte reads any player's copy, func_0203db3c writes this
   console's) become mode, the rest of player's byte kept. dScEntry_c::
   UpdateControlSettings calls it with this console's index right after
   SetControllerMode, so the other consoles see the choice. */
void func_02020028(int player, unsigned char mode) {
    int x = _Z17GetPlayerFlagByteii(player, 4);
    x = mode | (x & ~3);
    _Z13func_0203db3cii(4, x & 0xff);
}
