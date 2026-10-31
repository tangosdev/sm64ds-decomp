extern int func_0203da9c(void);
extern unsigned char GetPlayerFlagByte(int player, int byte);
extern void func_0203db3c(int byte, int value);

/* If playerNo is the local player's, clears bits 4-6 of its link
   settings byte 0, the field IsPlayerWarping reads. daPly_c::
   func_ov002_020c2e78 calls it when that field is set but the player
   is not in state data_ov002_02110154; otherwise it copies the field
   into mTeleportId. */
void func_02020388(int playerNo) {
    if (playerNo != func_0203da9c())
        return;
    int v = GetPlayerFlagByte(playerNo, 0);
    func_0203db3c(0, (unsigned char)(v & ~0x70));
}
