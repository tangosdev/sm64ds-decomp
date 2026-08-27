extern int _ZN4cstd3divEii(int,int);
int func_ov007_020b1f2c(int x, int* out){
  if(x>=0 && x<=0x3c){
    int r;
    if(x<=0) r=0;
    else if(x>=0x3c) r=0x1000;
    else r=_ZN4cstd3divEii(x<<0xc, 0x3c);
    int v=(int)(((long long)r*r)>>0xc);
    int v2=(int)(((long long)v*v)>>0xc);
    *out=0x1000-v2;
    return 1;
  }
  *out=0;
  return 0;
}
