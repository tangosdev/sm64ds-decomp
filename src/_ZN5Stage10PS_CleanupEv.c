extern unsigned char PAUSED_DURING_TIMER[];
extern int TIME_TIMER[];
extern unsigned char TOP_SCREEN_RELATED[];
extern unsigned char BOTTOM_SCREEN_RELATED[];
extern unsigned char STOP_MUSIC_DURING_FADE[];
extern unsigned short STAGE_TIMER[];
extern unsigned char GAME_PAUSED[];
extern int _ZN5Timer10StartTimerEv(void*);
extern int _ZN5Sound12UnpauseMusicEv(void);
extern int _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);
void _ZN5Stage10PS_CleanupEv(void){
  if(*PAUSED_DURING_TIMER){
    _ZN5Timer10StartTimerEv(TIME_TIMER);
    *PAUSED_DURING_TIMER=0;
  }
  *TOP_SCREEN_RELATED &= ~0xe;
  *BOTTOM_SCREEN_RELATED &= ~3;
  if(*STOP_MUSIC_DURING_FADE==0) _ZN5Sound12UnpauseMusicEv();
  else _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
  *STAGE_TIMER=0xf;
  *(short*)0x4000050=0;
  *(short*)0x4001050=0;
  *STOP_MUSIC_DURING_FADE=0;
  *GAME_PAUSED=0;
}
