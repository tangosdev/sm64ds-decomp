
extern short ReadUnalignedShort(unsigned char *p);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
int func_ov002_020f6e48(char *self, unsigned char *p)
{
  int b;
  int c0;
  int d;
  int e;
  int a;
  int f;
  unsigned char new_var;
  int rv;
  int new_var2;
  a = ReadUnalignedShort(p + 4) << 12;
  b = ReadUnalignedShort(p + 2) << 12;
  c0 = ReadUnalignedShort(p) << 12;
  *((int *) (self + 0x5c)) = c0;
  *((int *) (self + 0x60)) = b;
  *((int *) (self + 0x64)) = a;
  d = ReadUnalignedShort(p + 0xa);
  e = ReadUnalignedShort(p + 8);
  f = ReadUnalignedShort(p + 6);
  *((short *) (self + 0x8c)) = (short) f;
  *((short *) (self + 0x8e)) = (short) e;
  *((short *) (self + 0x90)) = (short) d;
  *((short *) (self + 0x92)) = *((short *) (self + 0x8c));
  *((short *) (self + 0x94)) = *((short *) (self + 0x8e));
  *((short *) (self + 0x96)) = *((short *) (self + 0x90));
  *((int *) (self + 0xf4)) = (new_var = p[0xc]) << 12;
  *((int *) (self + 0x80)) = 0xb33;
  *((int *) (self + 0x84)) = 0xb33;
  *((int *) (self + 0x88)) = 0xb33;
  rv = *((int *) ((*((int *) (self - -0xe0))) + 0x54));
  new_var2 = RandomIntInternal(&data_0209e650);
  *((int *) ((*((int *) (self + 0xe0))) + 0x58)) = ((unsigned short) (((int) (((((long long) ((int) ((((unsigned int) new_var2) >> 20) & 0xfff))) * rv) + 0x800) >> 12)) >> 12)) << 12;
  return 1;
}
