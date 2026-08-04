void func_ov006_020d5fd8(int c, int i){
  int p = c + (i<<4) + 0x6200;
  *(short*)(p+0xa8)=0;
}
