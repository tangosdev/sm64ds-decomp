extern int data_ov007_02103360[];
extern int data_ov007_0210342c[];
void func_ov007_020b3e84(short* r0){
  int a=*(int*)data_ov007_02103360[0];
  int c=data_ov007_0210342c[0];
  short idx=*(short*)a;
  int e=*(int*)(c+idx*4+0x114);
  int f=*(int*)e;
  int x=*(int*)(f+8);
  int y=*(int*)(f+0xc);
  r0[0]=x>>0xc;
  r0[1]=y>>0xc;
  r0[2]=1;
  r0[3]=0;
}
