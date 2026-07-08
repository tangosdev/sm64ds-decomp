//cpp
extern "C" {
extern int ShowArea(int);
extern int CAMERA[];
int _ZN6Camera9SetFlag_3Ev(void*);
extern int func_02012694(int, void*);
int func_ov100_02145ab4(char *c){
  ShowArea((signed char)*(short*)(c+0x8c));
  ShowArea((signed char)*(short*)(c+0x90));
  _ZN6Camera9SetFlag_3Ev((void*)CAMERA[0]);
  *(int*)(c+0xa0) = -0x8a000;
  func_02012694(0x4e, c+0x74);
  return 1;
}
}
