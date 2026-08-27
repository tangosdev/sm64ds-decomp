extern int data_ov004_020bf9ec;
extern char* data_ov004_020beb68;
extern int data_ov004_020bfa18;
extern void* data_ov004_020bfa20;
extern void func_ov004_020b612c(void);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);

void func_ov004_020b63a0(int r0){
  char* p;
  int v;
  if (data_ov004_020bf9ec != 0) return;
  p = data_ov004_020beb68;
  v = p ? *(int*)(p + 0xa8) : 0;
  if (r0 <= v) v = r0;
  data_ov004_020bfa18 += v;
  data_ov004_020bfa20 = (void*)func_ov004_020b612c;
  _ZN5Sound12PlayBank2_2DEj(0x14d);
  data_ov004_020bf9ec = 1;
}
