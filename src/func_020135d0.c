extern unsigned char SAVE_DATA[];
int func_020135d0(int r0){
  int res;
  if(r0 < 0x1c){
    res = (*(int*)(SAVE_DATA+0x4c) & (1<<r0)) ? 0 : 1;
  } else {
    res = (SAVE_DATA[0x50] & (1<<(r0-0x1c))) ? 0 : 1;
  }
  return res;
}
