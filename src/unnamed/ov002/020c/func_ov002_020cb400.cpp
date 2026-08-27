//cpp
extern "C" {
extern int func_02012194(int a0, int a1, int a2, int a3, int a4, int a5, int a6);
int func_ov002_020cb400(char* c){
  int v = *(short*)(c+0x600+0x9c) / 16;
  if (v >= 0x100) v = 0x100;
  int code = 0x107;
  if (*(int*)(*(int*)(c+0x37c)+0x18) & 0x1000000) code = 0x105;
  int r = func_02012194(*(int*)(c+0x620), 0, code, 3, v, (int)(c+0x74), 0);
  *(int*)(c+0x620) = r;
  return r;
}
}
