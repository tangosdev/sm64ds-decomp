void func_ov002_020de3d0(int a0, short* r1, short* r2){
  if(*r2 >= 0x100) *r2 -= 0x20;
  if(*r2 <= -0x100) *r2 += 0x20;
  if(*r1 < 0) *r2 += 0x10;
  else *r2 -= 0x10;
  *r1 = *r1 + *r2;
}
