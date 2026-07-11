//cpp
extern "C" {
extern void EnterBigBoosHaunt(void);
int func_ov002_020c8f0c(char* c){
  int *p0 = (int*)(((int)c + 0x80) & 0xFFFFFFFFFFFFFFFF);
  int *p1 = (int*)(((int)c + 0x84) & 0xFFFFFFFFFFFFFFFF);
  int *p2 = (int*)(((int)c + 0x88) & 0xFFFFFFFFFFFFFFFF);
  *p0 -= 0x80;
  *p1 -= 0x80;
  *p2 -= 0x80;
  if (*(int*)(c+0x80) < 0) {
    *(int*)(c+0x80) = 0;
    *(int*)(c+0x84) = 0;
    *(int*)(c+0x88) = 0;
  }
  if (*(unsigned char*)(c+0x6de) == 0) {
    EnterBigBoosHaunt();
    *(unsigned char*)(c+0x6e3) = 6;
  }
  return 0;
}
}
