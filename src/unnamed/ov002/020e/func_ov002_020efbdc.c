extern int func_ov002_020efedc(int* p);
extern void func_ov002_020efa54(void* c, int i);
void func_ov002_020efbdc(char* c){
  *(int*)(c+0x5c) = *(int*)(c+0x410);
  *(int*)(c+0x60) = *(int*)(c+0x414);
  *(int*)(c+0x64) = *(int*)(c+0x418);
  *(short*)(c+0x8c) = *(short*)(c+0x41c);
  *(short*)(c+0x8e) = *(short*)(c+0x41e);
  *(short*)(c+0x90) = *(short*)(c+0x420);
  *(short*)(c+0x92) = *(short*)(c+0x41c);
  *(short*)(c+0x94) = *(short*)(c+0x41e);
  *(short*)(c+0x96) = *(short*)(c+0x420);
  *(int*)(c+0x438) = 0;
  *(int*)(c+0x98) = *(int*)(c+0x440);
  if (func_ov002_020efedc((int*)c)) func_ov002_020efa54(c, 0);
  else func_ov002_020efa54(c, 1);
}
