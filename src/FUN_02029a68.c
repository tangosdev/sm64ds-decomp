extern int WIPES;
extern int KS_FADER;
extern unsigned char CAMERA_SAVED_LOOK_AT_RELATED;

void FUN_02029a68(void){
  if (KS_FADER == WIPES + 0x240) {
    KS_FADER = 0;
    *(volatile unsigned short*)0x4000050 = 0;
  }
  CAMERA_SAVED_LOOK_AT_RELATED = 0;
}
