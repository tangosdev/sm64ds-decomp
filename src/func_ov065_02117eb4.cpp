//cpp
// @symbol func_ov065_02117eb4
// @emits Swoop_OnTurnIntoEgg
/* recovered: renamed to Class_Method */
/* daBasabasa_c::OnTurnIntoEgg - recovered from vtable slot identity */
extern "C" {
int _ZN5Actor15GivePlayerCoinsER6Playerhj(void *thisp, void *player, unsigned char count, unsigned int z);
int _ZN5Actor24KillAndTrackInDeathTableEv(void *thisp);
int Swoop_OnTurnIntoEgg(char *c, void *player) {
    _ZN5Actor15GivePlayerCoinsER6Playerhj(c, player, (unsigned char)(*(unsigned char*)(c+0x10a)+1), 0);
    return _ZN5Actor24KillAndTrackInDeathTableEv(c);
}
}
