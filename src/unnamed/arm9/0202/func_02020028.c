extern unsigned char GetLinkByte(int slot, int byte);
extern void SetLinkByte(int byte, int value);

/* The writer twin of GetControllerMode: bits 0-1 of link settings byte 4
   (GetLinkByte reads any slot's copy, SetLinkByte writes this
   console's) become mode, the rest of player's byte kept. dScEntry_c::
   UpdateControlSettings calls it with this console's index right after
   SetControllerMode, so the other consoles see the choice. */
void func_02020028(int player, unsigned char mode) {
    int x = GetLinkByte(player, 4);
    x = mode | (x & ~3);
    SetLinkByte(4, x & 0xff);
}
