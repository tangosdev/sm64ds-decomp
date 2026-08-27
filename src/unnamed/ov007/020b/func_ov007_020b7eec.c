
extern void func_ov007_020c0308(void *dst, int r1, int v);
extern void MultiStore32Bytes(unsigned val, int *dst, int len);
extern int func_ov007_020b8ec0(int x);
extern int func_ov007_020c94a0(int a, unsigned int b);
extern void func_ov007_020b8b80(void *a, int b, int c);
extern short data_ov007_02103998;
extern short data_ov007_02104598;
extern int *data_ov007_02103478[];
extern int data_ov007_020d7a5c[];
extern int data_ov007_02102dd8;
extern int data_ov007_02103464;
extern int data_ov007_02103468;
extern int data_ov007_020d7a50[];
void func_ov007_020b7eec(unsigned char *t)
{
  int r;
  func_ov007_020c0308(&data_ov007_02103998, 0x18, 0xf);
  func_ov007_020c0308(&data_ov007_02104598, 0x18, 0xf);
  {
    volatile int z = 0;
    MultiStore32Bytes((unsigned) z, data_ov007_02103478[0], data_ov007_020d7a5c[0]);
  }
  data_ov007_02102dd8 = 0xb;
  data_ov007_02103464 = 0xac;
  data_ov007_02103468 = 4;
  r = func_ov007_020b8ec0(data_ov007_020d7a50[t[8]]);
  r = func_ov007_020c94a0(0, (unsigned int) r);
  func_ov007_020b8b80(0, r, 0);
  data_ov007_02103464 = 0x8c;
  data_ov007_02103468 = 0xa;
  if (t[9] != 0)
  {
    data_ov007_02102dd8 = 0xb;
  }
  else
  {
    data_ov007_02102dd8 = 0xc;
  }
  r = func_ov007_020b8ec0(0x1f51);
  r = func_ov007_020c94a0(0, (unsigned int) r);
  func_ov007_020b8b80(0, r, 0);
  data_ov007_02103464 = 0xcc;
  data_ov007_02103468 = 0xa;
  r = data_ov007_02102dd8;
  if (r == 0xb)
  {
    data_ov007_02102dd8 = 0xc;
  }
  else
  {
    data_ov007_02102dd8 = 0xb;
  }
  r = func_ov007_020b8ec0(0x1f52);
  r = func_ov007_020c94a0(0, (unsigned int) r);
  func_ov007_020b8b80(0, r, 0);
}
