//cpp
// @symbol func_ov084_0212e9d4
// @emits FirePiranhaPlantBig_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* daFPkn_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern "C" {
int _ZN5Actor15GivePlayerCoinsER6Playerhj(void *thisp, void *player, unsigned char count, unsigned int z);
int _ZN5Actor24KillAndTrackInDeathTableEv(void *thisp);
int FirePiranhaPlantBig_OnTurnIntoEgg(void *c, void *player) {
    _ZN5Actor15GivePlayerCoinsER6Playerhj(c, player, 1, 0);
    return _ZN5Actor24KillAndTrackInDeathTableEv(c);
}
}
