//cpp
extern "C" {
int _ZN5Actor15GivePlayerCoinsER6Playerhj(void *thisp, void *player, unsigned char count, unsigned int z);
int _ZN5Actor24KillAndTrackInDeathTableEv(void *thisp);
int func_ov090_02132620(char *c, void *player) {
    _ZN5Actor15GivePlayerCoinsER6Playerhj(c, player, (unsigned char)(*(unsigned char*)(c+0x10a)+1), 0);
    return _ZN5Actor24KillAndTrackInDeathTableEv(c);
}
}
