#pragma opt_common_subs off
extern int data_ov007_02103444;
extern int data_ov007_02103448;
int func_ov007_020b78d0(int x){
  int r3 = x << 1;
  int r0 = x & ~3;
  int b = data_ov007_02103448;
  int a = data_ov007_02103444;
  int t = r3 - r0;
  int st = a + t;
  int rv = b + a;
  data_ov007_02103444 = st;
  return rv;
}