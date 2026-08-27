void func_ov006_020d6264(int c, int i){
  int p = c + (i<<4) + 0x6200;
  *(short*)(p+0x88)=0;
}
