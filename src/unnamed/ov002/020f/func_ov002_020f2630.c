extern void* _Znwj(unsigned int);
extern void SetSubBg0Offset(int a, int b);
extern void SetSubBg1Offset(int a, int b);
int func_ov002_020f2630(char* c){
  short* p;
  *(void**)(c+0xd4) = _Znwj(0x36);
  (*(short**)(c+0xd4))[0] = 0x30;
  (*(short**)(c+0xd4))[2] = 0x20;
  (*(short**)(c+0xd4))[1] = 0;
  (*(short**)(c+0xd4))[3] = 0;
  (*(unsigned short**)(c+0xd4))[4] = 0xfff8;
  (*(short**)(c+0xd4))[5] = 0xc8;
  (*(short**)(c+0xd4))[0x17] = 0x10;
  p = *(short**)(c+0xd4); SetSubBg0Offset(p[0], p[2]);
  p = *(short**)(c+0xd4); SetSubBg1Offset(p[1], p[3]);
  return 1;
}
