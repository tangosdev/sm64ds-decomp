extern int _ZN4cstd3divEii(int,int);
int func_ov007_020aeaec(char* c, int flag, int lo, int hi){
  int v = *(int*)(*(int*)(c+4)+0xc);
  int r;
  if(v<=lo) r=0;
  else if(v>=hi) r=0x1000;
  else r=_ZN4cstd3divEii((v-lo)<<0xc, hi-lo);
  if(flag) r=0x1000-r;
  return r;
}
