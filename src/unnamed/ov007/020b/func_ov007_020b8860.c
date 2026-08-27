extern int data_ov007_02102ddc[];
extern int* data_ov007_02103478;
extern int data_ov007_020d7a5c;
extern void MultiStore32Bytes(unsigned val, int* dst, int len);
extern void func_ov007_020c1d8c(int i);
void func_ov007_020b8860(int r0, int r1){
  volatile int z = 0;
  data_ov007_02102ddc[1] = r1;
  data_ov007_02102ddc[0] = r0;
  data_ov007_02102ddc[2] = 0;
  data_ov007_02102ddc[3] = 0;
  data_ov007_02102ddc[4] = 0;
  MultiStore32Bytes((unsigned)z, data_ov007_02103478, data_ov007_020d7a5c);
  func_ov007_020c1d8c(0);
}
