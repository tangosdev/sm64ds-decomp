
typedef unsigned char u8;
typedef unsigned short u16;
extern int _ZN3IRQ7DisableEv(void);
extern void _ZN3IRQ7RestoreEj(int state);
extern void MultiStore_Int(int val, int *dst, int len);
extern void MultiStore16(int val, char *dst, int nbytes);
extern void func_0205a61c(u16 *a, char *b, int c);
extern int func_02069038(void);
extern char *data_020a9db8;
extern int data_020a9db4;
extern short data_020a9db0;
extern u16 *data_0209a084;
int func_02068e4c(int a0, u16 *a1, int a2, int a3, u16 a4, int a5)
{
  volatile unsigned short fill_s;
  volatile int fill_i;
  int base1;
  int base2;
  int irq;
  if ((data_020a9db8 != 0) && ((*((u16 *) (data_020a9db8 + 0x1316))) != 0))
  {
    return 2;
  }
  base1 = (a0 + 0x1f) & (~0x1f);
  base2 = (base1 + 0x371f) & (~0x1f);
  irq = _ZN3IRQ7DisableEv();
  fill_i = 0;
  data_020a9db0 = (short) a5;
  data_020a9db4 = base1;
  data_020a9db8 = (char *) base2;
  MultiStore_Int(fill_i, (int *) base1, 0x3700);
  fill_s = 0;
  MultiStore16(fill_s, (char *) base2, 0x1340);
  {
    int i = 0;
    u16 *dst = (u16 *) (base1 + 0x1e30);
    u16 *rp = a1;
    u16 v;
    do
    {
      v = *((rp + i) + 1);
      if (v == 0)
      {
        break;
      }
      i++;
      *(dst++) = v;
    }
    while (i < 0xa);
  }
  {
    u16 *dst = (u16 *) (base1 + 0x1e38);
    int j = 0;
    do
    {
      u16 *ptr = data_0209a084;
      if ((*ptr) == 0)
      {
        break;
      }
      data_0209a084 = ptr + 1;
      j++;
      *(dst++) = *ptr;
    }
    while (j < 0x10);
  }
  func_0205a61c(a1, (char *) (base2 + 0x1300), 0x16);
  *((u16 *) (base1 + 0x1400)) = 0x100;
  *((u16 *) (base1 + 0x1402)) = 8;
  *((u16 *) (base1 + 0x1e18)) = 0;
  *((u16 *) (base1 + 0x1e1a)) = 0;
  *((u16 *) (base1 + 0x1e2c)) = 1;
  *((int *) (base1 + 0x1404)) = base2;
  *((int *) (base1 + 0x1408)) = base2 + 0x400;
  {
    char *hdr = (char *) (base1 + 0x1300);
    *((u16 *) (hdr + 0x14)) = 1;
    *((u16 *) (hdr + 0x12)) = 0;
    *((u16 *) (hdr + 6)) = (short) a2;
    *((u16 *) (hdr + 8)) = (short) a3;
    *((u16 *) (hdr + 0xa)) = (short) a4;
    *((u16 *) (hdr + 0xc)) = 1;
    *((u16 *) (hdr + 0x10)) = 0;
    *((u16 *) (hdr + 0x16)) = (u16) (func_02069038() + 0xc8);
    *((u16 *) (hdr + 0xe)) = 0xf;
  }
  *((u8 *) (base1 + 0x1410)) = 0;
  *((u8 *) (base1 + 0x1411)) = 0;
  *((int *) (base2 + 0x1324)) = 0;
  _ZN3IRQ7RestoreEj(irq);
  *((u16 *) (base2 + 0x1316)) = 1;
  return 0;
}
