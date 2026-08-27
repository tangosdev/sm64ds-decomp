extern int data_ov007_02103370[];
extern int data_ov007_0210336c[];
void func_ov007_020b3edc(int r0){
  int p=data_ov007_02103370[r0];
  if(p==0) return;
  ((void(*)(int))data_ov007_0210336c[0])(p);
  data_ov007_02103370[r0]=0;
}
