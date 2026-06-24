extern unsigned char data_ov066_0211abe0[];
int func_ov066_02116b78(char* c){
  int b1 = 0x320;
  int lo = -0x320;
  int ip = *(int*)(c+0x49c);
  if(ip == 2){ b1 = 0x384; lo = -0x384; }
  unsigned char flag = data_ov066_0211abe0[0];
  int v = *(int*)(c+0x5c);
  if(flag != 3) lo -= 0x50;
  lo = lo << 0xc;
  if(v < lo){
    *(int*)(c+0x5c) = lo;
    *(int*)(c+0x98) = 0;
    return 1;
  }
  b1 = 0x320;
  if(ip == 1) b1 = 0x384;
  if(flag != 3) b1 += 0x50;
  b1 = b1 << 0xc;
  if(v > b1){
    *(int*)(c+0x5c) = b1;
    *(int*)(c+0x98) = 0;
    return 1;
  }
  int z = *(int*)(c+0x64);
  int n = 0xff0ff000;
  if(z < n){
    *(int*)(c+0x64) = n;
    *(int*)(c+0x98) = 0;
    return 1;
  }
  n = 0xff8c6000;
  if(flag != 3) n += 0x8c000;
  if(z > n){
    *(int*)(c+0x64) = n;
    *(int*)(c+0x98) = 0;
    return 1;
  }
  return 0;
}
