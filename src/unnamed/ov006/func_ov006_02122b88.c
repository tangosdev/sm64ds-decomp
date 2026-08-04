
extern int data_ov006_02142f74;
extern unsigned char *data_ov006_02142f7c;
extern void func_ov006_021228bc(void *, void *);
void func_ov006_02122b88(unsigned char *c)
{
  int i = 0;
  int n = data_ov006_02142f74;
  unsigned char *base;
  unsigned char *p;
  if (n <= 0)
  {
    return;
  }
  base = data_ov006_02142f7c;
  p = base;
  do
  {
    if ((*((short *) (p + 0x76))) == 0)
    {
      func_ov006_021228bc(base + (i * 0x78), c);
      return;
    }
    i++;
    p += 0x78;
  }
  while (i < data_ov006_02142f74);
}
