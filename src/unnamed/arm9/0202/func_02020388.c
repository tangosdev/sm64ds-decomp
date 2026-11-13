extern int _Z13func_0203da9cv(void);
extern unsigned char _Z17GetPlayerFlagByteii(int player, int byte);
extern void _Z13func_0203db3cii(int byte, int value);

/* If playerNo is the local player's, clears bits 4-6 of its link
   settings byte 0, the field IsPlayerWarping reads. daPly_c::
   func_ov002_020c2e78 calls it when that field is set but the player
   is not in state data_ov002_02110154; otherwise it copies the field
   into mTeleportId. */
void func_02020388(int playerNo) {
    if (playerNo != _Z13func_0203da9cv())
        return;
    int v = _Z17GetPlayerFlagByteii(playerNo, 0);
    _Z13func_0203db3cii(0, (unsigned char)(v & ~0x70));
}
