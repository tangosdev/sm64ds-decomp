typedef unsigned short u16;
extern unsigned char CURRENT_GAMEMODE;
extern int DP_STATE;
extern unsigned char VS_MODE_COUNTDOWN;
extern u16 VS_MODE_TIMER_TIMER;
extern unsigned char VS_TIME_UP;
int _ZN5Stage8CanPauseEv(void){
    if (((unsigned)(CURRENT_GAMEMODE == 1)) == 0) return 1;
    if (DP_STATE != 0) return 0;
    if (VS_MODE_COUNTDOWN != 0 || (VS_MODE_TIMER_TIMER == 0 && VS_TIME_UP != 0)) return 0;
    return 1;
}
