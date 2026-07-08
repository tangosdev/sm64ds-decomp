extern unsigned char VS_MODE_COUNTDOWN;
extern short VS_NEW_STAR_TIMER;
extern unsigned char VS_NEW_STAR_SOUND_PLAYED;

void FUN_0202a130(void) {
    if (VS_MODE_COUNTDOWN != 0) return;
    VS_NEW_STAR_TIMER = 0xb4;
    VS_NEW_STAR_SOUND_PLAYED = 0;
}
