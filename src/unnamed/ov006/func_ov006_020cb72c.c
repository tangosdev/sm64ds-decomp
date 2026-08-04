
extern void func_ov006_020cb528(char *c);
extern void func_ov006_020cc8c8(char *c);
extern int func_ov006_020e6e3c(int a, int b);
extern void func_ov006_020ccc8c(char *c);
extern int data_ov006_021405c8[];
extern short data_ov006_02140538;
void func_ov006_020cb72c(char *c)
{
  int v = *((int *) (c + 0x1c));
  if (v > 0)
  {
    short idx = *((short *) (c + 0x52));
    int cur = *((int *) (c + 0x20));
    int *d = data_ov006_021405c8;
    if (cur <= d[idx])
    {
      if (idx == data_ov006_02140538)
      {
        func_ov006_020cb528(c);
      }
      else
      {
        func_ov006_020cc8c8(c);
      }
      func_ov006_020e6e3c(0x1b4, *((int *) (c + 0x1c)));
    }
    else
    {
      int lim = d[1] + 0x30000;
      if ((cur >= lim) && (idx == 1))
      {
        *((int *) (c + 0x20)) = lim;
      }
    }
  }
  else
  {
    if ((*((int *) (c + 0x20))) <= data_ov006_021405c8[0])
    {
      func_ov006_020e6e3c(0x1b4, v);
      func_ov006_020ccc8c(c);
      return;
    }
  }
  {
    int w = *((int *) (c + 0x1c));
    if (w > 0x70000)
    {
      *((int *) (c + 0x1c)) = 0x70000;
      *((int *) (c + 0x34)) = 0;
      return;
    }
    if (w < (-0x70000))
    {
      *((int *) (c + 0x1c)) = -0x70000;
      *((int *) (c + 0x34)) = 0;
    }
  }
}
