//cpp
struct OamAttr; struct Matrix2x2;
extern "C" {
extern unsigned char CURR_PLAYER_ID[];
extern signed char NUM_VS_STARS_OBTAINED_PLAYER[];
extern signed char NUM_RED_COINS[];
extern struct OamAttr _ZN3OAM8RED_COINE;
void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(bool, struct OamAttr*, int, int, int, int, struct Matrix2x2*);
}
extern "C" void _ZN3HUD14RenderRedCoinsEv(){
  int idx = CURR_PLAYER_ID[0];
  int flag = NUM_VS_STARS_OBTAINED_PLAYER[idx];
  int count = (unsigned char)NUM_RED_COINS[idx];
  int sl = flag ? 0x1c : 0xa;
  for(int i=0;i<count;i++){
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(false, &_ZN3OAM8RED_COINE, 0x10 + i*0xb, sl, -1, 1, (struct Matrix2x2*)0);
  }
}
