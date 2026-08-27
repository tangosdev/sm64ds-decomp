void func_ov007_020c0288(unsigned short* p, int count, int val){
  int i;
  for(i=0;i<count;i++){
    *p = (val<<12) | (*p & 0xfff);
    p++;
  }
}
