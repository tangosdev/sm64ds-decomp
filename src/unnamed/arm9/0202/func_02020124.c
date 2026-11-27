extern int func_0203da9c(void);
extern unsigned char GetLinkByte(int slot, int byte);
extern void SetLinkByte(int byte, int value);
/* daPly_c 0 only: sets the go flag, bit 5 of link settings byte 2. The
   dScEntry_c sync steps raise it once ArePlayersReady (or
   ArePlayersReadyForRematch) sees every joined player's ready bit; each
   console then advances when func_02020168 sees it. */
void func_02020124(void) {
    int x;
    if (func_0203da9c() != 0)
        return;
    x = GetLinkByte(0, 2);
    SetLinkByte(2, (unsigned char)(x | 0x20));
}
