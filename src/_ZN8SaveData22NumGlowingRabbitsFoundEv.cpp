//cpp
extern "C" {
extern unsigned char SAVE_DATA[];
int _ZN8SaveData22NumGlowingRabbitsFoundEv(void){
  int count = 0;
  int f = *(int*)(SAVE_DATA+8);
  unsigned int mask = 0x100000;
  int i = 0;
  do {
    i++;
    if(f & mask) count++;
    mask <<= 1;
  } while(i < 8);
  return count;
}
}
