struct S { char pad[0x4c]; int w; unsigned char b; };
extern struct S data_0209caa0;
void func_02013598(int x){
  if (x < 0x1c)
    data_0209caa0.w |= 1 << x;
  data_0209caa0.b |= 1 << (x - 0x1c);
}
