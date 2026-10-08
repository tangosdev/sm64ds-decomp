extern unsigned char data_0209caa0[];
int func_020135d0(int r0){
  int res;
  if(r0 < 0x1c){
    res = (*(int*)(data_0209caa0+0x4c) & (1<<r0)) ? 0 : 1;
  } else {
    res = (data_0209caa0[0x50] & (1<<(r0-0x1c))) ? 0 : 1;
  }
  return res;
}
