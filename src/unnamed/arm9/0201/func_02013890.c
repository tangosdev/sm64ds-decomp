extern unsigned char data_0209caa0[];
int func_02013890(int r0, int r1){
  if(r1 < 0){
    int f = *(int*)(data_0209caa0+0xc);
    int m = 0xf << (r0*4);
    return (f & m) == m;
  } else {
    int f = *(int*)(data_0209caa0+0xc);
    return f & (1 << (r1 | (r0<<2)));
  }
}
