struct S { char pad[0x4c]; int w; unsigned char b; };
extern struct S SAVE_DATA;
void func_02013598(int x){
  if (x < 0x1c)
    SAVE_DATA.w |= 1 << x;
  SAVE_DATA.b |= 1 << (x - 0x1c);
}
