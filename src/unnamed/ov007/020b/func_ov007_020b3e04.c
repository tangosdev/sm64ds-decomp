typedef short s16;

void func_ov007_020b3e04(int sel, s16 *p1, s16 *p2){
  *p2 = 0;
  *p1 = *p2;
  switch(sel){
  case 0:
    *p2 = *p2 - 0x16;
    break;
  case 1:
    *p2 = 0x10;
    break;
  case 2:
    *p1 = 0x20;
    break;
  case 3:
    *p2 = -0x16;
    break;
  case 4:
    *p2 = -0x10;
    break;
  case 5:
    *p1 = -0x19;
    break;
  }
}
