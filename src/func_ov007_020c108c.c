void func_ov007_020c108c(char *c, int r1, int r2, int r3, int sp0){
  *(char*)(c+0x1d) = (char)r1;
  if(sp0 >= 0){
    *(short*)(c+0x2c) = (short)sp0;
    *(short*)(c+0x2e) = 0;
  }
  *(int*)(c+0x20) = r2;
  *(int*)(c+0x24) = r3;
}
