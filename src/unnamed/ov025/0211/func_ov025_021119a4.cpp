//cpp
extern "C" {
int _Z14ApproachLinearRsss(short &a, short b, short c);
int func_ov025_021119a4(char *c){
  int r = _Z14ApproachLinearRsss(*(short*)(c+0x8e), *(short*)(c+0x39c), 0x400);
  if(r==0) return r;
  *(short*)(c+0x94) = *(short*)(c+0x8e);
  *(int*)(c+0x398) = 6;
  *(unsigned char*)(c+0x39f) = 0;
  *(unsigned char*)(c+0x39e) = 0x28;
  return 0x28;
}
}
