void func_ov007_020c1404(int *c){
  unsigned int *reg = (unsigned int*)0x40004ac;
  if(c){
    int t = (c[1]==2) ? 1 : 0;
    *reg = (unsigned)c[9] >> (4 - t);
  } else {
    *reg = 0;
  }
}
