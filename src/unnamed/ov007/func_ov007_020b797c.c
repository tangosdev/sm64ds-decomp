extern int data_ov007_02103478;
extern int data_ov007_020d7a5c;
extern char data_ov007_02102ddc[];
extern void MultiStore32Bytes(int,int,int);
void func_ov007_020b797c(void){
  volatile int x=0;
  MultiStore32Bytes(x, data_ov007_02103478, data_ov007_020d7a5c);
  *(int*)(data_ov007_02102ddc+0x14)=0;
}
