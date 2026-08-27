
inline char *inline_fn(char *arg0)
{
  return arg0 + 0x448;
}

void func_ov084_021290d4(char *c)
{
  int new_var;
  char *new_var2;
  char *base = c + 0x400;
  int new_var3;
  new_var = 0x458;
  if ((*((unsigned short *) (base + 0x58))) != 0)
  {
    new_var3 = (*((int *) (c + 0x434))) == 1;
    if (new_var3)
    {
      *((unsigned short *) ((c + 0x400) + 0x58)) = 0;
    }
    else
    {
      *((unsigned short *) ((((int) c) + new_var))) = (*((unsigned short *) ((((int) c) + 0x458)))) - 1;
    }
    return;
  }
  if ((*((unsigned char *) ((new_var2 = c) + 0x113))) < 6)
  {
    *((int *) inline_fn(new_var2)) = 0x1f4000;
    return;
  }
  unsigned short v = *((unsigned short *) ((c + 0x400) + 0x56));
  if (v > 0xa)
  {
    *((int *) inline_fn(new_var2)) = 0x1f4000 - (((v - 0xa) * 0x14) << 12);
    if ((*((int *) inline_fn(new_var2))) < 0xa000)
    {
      *((int *) inline_fn(new_var2)) = 0xa000;
    }
    return;
  }
  *((int *) inline_fn(new_var2)) = 0x1f4000;
}
