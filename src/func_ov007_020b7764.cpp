//cpp
extern "C" {
extern int data_ov007_02103448;
void func_ov007_020b3edc(int);
void func_ov007_020c3d1c(void *);
void func_ov007_020b7764(void){
  func_ov007_020b3edc(0x17);
  if(data_ov007_02103448==0) return;
  func_ov007_020c3d1c((void *)data_ov007_02103448);
  data_ov007_02103448=0;
}
}
