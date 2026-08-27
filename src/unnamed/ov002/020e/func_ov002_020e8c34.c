
extern void func_ov002_020e8abc(void);
volatile unsigned int func_ov002_020e8c34(void *a)
{
  int v;
  int y;
  v = *((int *) (((char *) a) + 0x5c));
  y = 0;
  if (y > v)
  {
    v = -v;
  }
  if (v <= 0x13880000)
  {
    v = *((int *) (((char *) a) + 0x64));
    if (v < y)
    {
      v = -v;
    }
    if (v <= 0x13880000)
    {
      y = *((int *) (((char *) a) + 0x60));
      if ((y >= (*((int *) (((char *) a) + 0x484)))) && (y <= 0x13880000))
      {
        return;
      }
    }
  }
  func_ov002_020e8abc();
}
