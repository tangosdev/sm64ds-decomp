extern int _Z13func_0203da9cv(void);
extern unsigned char _Z17GetPlayerFlagByteii(int player, int byte);
extern void _Z13func_0203db3cii(int byte, int value);
/* daPly_c 0 only: sets the go flag, bit 5 of link settings byte 2. The
   dScEntry_c sync steps raise it once ArePlayersReady (or
   ArePlayersReadyForRematch) sees every joined player's ready bit; each
   console then advances when func_02020168 sees it. */
void func_02020124(void) {
    int x;
    if (_Z13func_0203da9cv() != 0)
        return;
    x = _Z17GetPlayerFlagByteii(0, 2);
    _Z13func_0203db3cii(2, (unsigned char)(x | 0x20));
}
