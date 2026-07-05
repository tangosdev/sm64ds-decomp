
extern int func_ov004_020b56c8(int);
extern short data_ov004_020bf9e4;
struct S5300
{
  char pad[0x22];
  unsigned short timer;
  char pad2[0x2a - 0x24];
  unsigned short f2a;
};
void func_ov006_020f47d8(char *c)
{
  struct S5300 *s;
  if (((struct S5300 *) (c + 0x5300))->timer != 0)
  {
    unsigned short *ptr = (unsigned short *) ((((int) c) + 0x5322) & 0xFFFFFFFFFFFFFFFFULL);
    *ptr = (*ptr) - 1;
    if (((struct S5300 *) (c + 0x5300))->timer != 0)
    {
      return;
    }
    {
      unsigned char b = *((unsigned char *) (c + 0x5337));
      if (b < ((struct S5300 *) (c + 0x5300))->f2a)
      {
        return;
      }
    }
    {
      int val = 6;
      val = (val * (3 - (*((unsigned char *) (c + 0x533a))))) & 0xFFFFFFFFFFFFFFFFu;
      func_ov004_020b56c8(val);
      return;
    }
  }
  if (data_ov004_020bf9e4 == 1)
  {
    ((struct S5300 *) (c + 0x5300))->timer = 0;
    *((int *) (c + 0x5318)) = 2;
  }
}
