
extern int func_ov075_0211524c(void *unused, int idx);
extern void func_ov075_02114904(void *p, int x);
extern unsigned char data_0209fc5c[];
void func_ov075_02114fa8(void *thiz)
{
  int i;
  char *c = (char *) thiz;
  unsigned char *d = data_0209fc5c;
  char *base;
  base = c + 0x920;
  for (i = 0; i < 4; i++)
  {
    if ((*d) != 0)
    {
      int x = func_ov075_0211524c(thiz, i);
      func_ov075_02114904(base, x);
    }
    d++;
    base += 0x158;
  }

  if (data_0209fc5c[2] != 0)
  {
    *((unsigned char *) (c + 0xf42)) = 2;
  }
  else
    if (data_0209fc5c[1] != 0)
  {
    *((unsigned char *) (c + 0xf42)) = 1;
  }
  else
    if (data_0209fc5c[3] != 0)
  {
    *((unsigned char *) (c + 0xf42)) = 3;
  }
  else
  {
    *((unsigned char *) (c + 0xf42)) = 0;
  }
  *((unsigned char *) (c + 0xf40)) = 0;
  *((unsigned char *) (c + 0xf41)) = 1;
}
