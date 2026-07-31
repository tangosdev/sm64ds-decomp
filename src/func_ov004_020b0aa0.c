
extern void func_ov004_020b3194(void *c);
struct E
{
  char pad[0x20];
  int f20;
  char pad2[0x134 - 0x24];
};
extern struct E data_ov004_020bf648[];
void func_ov004_020b0aa0(int arg)
{
  int i;
  struct E *p;
  i = arg == 0x1d;
  if (i)
  {
    p = data_ov004_020bf648;
    for (i = 0; i < 3; i++)
    {
      if (p->f20 != 0x1d)
      {
        func_ov004_020b3194(p);
      }
      p++;
    }

  }
  else
  {
    p = data_ov004_020bf648;
    for (i = 0; i < 3; i++)
    {
      if (arg == p->f20)
      {
        func_ov004_020b3194(p);
      }
      p++;
    }

  }
}
