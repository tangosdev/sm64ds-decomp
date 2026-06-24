extern int _ZN4cstd4fdivEii(int a, int b);
short func_ov075_0211aeb8(char* c, int x){
  int t = _ZN4cstd4fdivEii(0xffff, 0x16c000);
  int v = (int)(((long long)x * t + 0x800) >> 12);
  return (short)(v - *(short*)(c+0xa0));
}
