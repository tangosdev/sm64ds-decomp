void func_ov006_020d65b4(int c, int i){
  int p = c + (i<<4) + 0x6200;
  *(short*)(p+0x68)=0;
}
