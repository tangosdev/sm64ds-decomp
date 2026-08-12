//cpp
// @symbol func_ov084_0212e9d4
// recovered name: FirePiranhaPlantBig_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* daFPkn_c::OnTurnIntoEgg - verified 2004/b56 byte-match (ov084), strict-reloc */
extern "C" {
int _ZN5Actor15GivePlayerCoinsER6Playerhj(void *thisp, void *player, unsigned char count, unsigned int z);
int _ZN5Actor24KillAndTrackInDeathTableEv(void *thisp);
int func_ov084_0212e9d4(void *c, void *player) {
    _ZN5Actor15GivePlayerCoinsER6Playerhj(c, player, 1, 0);
    return _ZN5Actor24KillAndTrackInDeathTableEv(c);
}
}
