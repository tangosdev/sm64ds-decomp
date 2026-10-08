void func_ov007_020b22e0(int* r0, int* r1, int r2){
  int a=r1[6];
  int ia=a+r2*20;
  int b=r1[8];
  int ib=b+(r2<<4);
  r0[2]=*(int*)(ia+4);
  r0[7]=*(int*)(ib+4);
}
