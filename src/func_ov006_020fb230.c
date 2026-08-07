typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
extern int RandomIntInternal(int *seed);
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);
extern int data_0209d4b8;
void func_ov006_020fb230(char *p, int i)
{
  int n = i * 0x1c;
  int v;
  u8 flag;
  *((int *) ((p + 0x5bc0) + n)) = func_02012468(*((int *) ((p + 0x5bc0) + n)), 2, 0x189, 4, 0, 0, func_020126e8(*((int *) ((p + 0x5bb0) + n))), 0);
  *((int *) ((p + 0x5bb0) + n)) += *((int *) ((p + 0x5bb8) + n));
  flag = *((u8 *) ((p + 0x5bc9) + n));
  v = (*((int *) ((p + 0x5bb0) + n))) >> 12;
  if (flag != 0)
  {
    if (v >= 0xc0)
      *((int *) ((p + 0x5bb8) + n)) -= 0x80;
    else if ((*((int *) ((p + 0x5bb8) + n))) <= 0x3000)
      *((int *) ((p + 0x5bb8) + n)) += 0x200;
    if (v >= 0xf0)
    {
      u32 a;
      if ((*((u8 *) ((p + 0x5bca) + n))) == 0) { *((u8 *) ((p + 0x5bc8) + n)) = 4; return; }
      *((int *) ((p + 0x5bb0) + n)) = 0xf0000;
      *((int *) ((p + 0x5bb8) + n)) = 0;
      *((u8 *) ((p + 0x5bc9) + n)) ^= 1;
      a = (((u32) RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
      *((u16 *) ((p + 0x5bc4) + n)) = (u16) ((((a << 3) >> 15) * 2) + 0x10);
      *((u8 *) ((p + 0x5bc8) + n)) = 3;
      return;
    }
  }
  else
  {
    if (v <= 0x40)
      *((int *) ((p + 0x5bb8) + n)) += 0x80;
    else if ((*((int *) ((p + 0x5bb8) + n))) >= (-0x3000))
      *((int *) ((p + 0x5bb8) + n)) -= 0x200;
    if (v <= 0x10)
    {
      u32 a;
      if ((*((u8 *) ((p + 0x5bca) + n))) == 0) { *((u8 *) ((p + 0x5bc8) + n)) = 4; return; }
      *((int *) ((p + 0x5bb0) + n)) = 0x10000;
      *((int *) ((p + 0x5bb8) + n)) = 0;
      *((u8 *) ((p + 0x5bc9) + n)) ^= 1;
      a = (((u32) RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
      *((u16 *) ((p + 0x5bc4) + n)) = (u16) ((((a << 3) >> 15) * 2) + 0x10);
      *((u8 *) ((p + 0x5bc8) + n)) = 3;
      return;
    }
  }
}
