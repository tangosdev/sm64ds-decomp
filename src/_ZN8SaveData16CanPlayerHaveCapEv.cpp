//cpp
extern "C" {
extern unsigned char SAVE_DATA[];
extern unsigned char CURRENT_GAMEMODE[];
int _ZN8SaveData16CanPlayerHaveCapEv(void){
  if (SAVE_DATA[0x41] != 3) {
    int b = (int)(CURRENT_GAMEMODE[0] == 1);
    if (b == 0) return 1;
  }
  return 0;
}
}
