extern int data_ov007_0210342c[];
void func_ov007_020b647c(int r0,int r1,short r2){
  if(r0==r1) return;
  int base=data_ov007_0210342c[0];
  int a=base+(r1<<2);
  int t=*(int*)(a+0x114);
  t=*(int*)t;
  t=*(int*)(t+4);
  *(short*)(t+2)=r2;
}
