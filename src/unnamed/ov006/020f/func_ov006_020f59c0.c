
void func_ov004_020ad79c(int r0arg, int r1arg);
inline int *inline_fn(char *arg0)
{
  return (int *) arg0;
}

void func_ov006_020f59c0(char *c)
{
  if ((*((unsigned char *) (c + 0x5405))) >= (*((unsigned short *) (c + 0x53ea))))
  {
    *((short *) (c + 0x53e4)) = 0x40;
    *inline_fn(c + 0x53d4) = 3;
    *inline_fn(c + 0x53d8) = 0;
    func_ov004_020ad79c(((5 - (*((unsigned char *) (c + 0x5408)))) * 5) + (*inline_fn(c + 0xa8)), (*inline_fn(c + 0xb4)) + 1);
  }
  if ((*((unsigned char *) (c + 0x5408))) >= (*((unsigned char *) (c + 0x5409))))
  {
    *((short *) (c + 0x53e4)) = 0x40;
    *inline_fn(c + 0x53d4) = 3;
    *inline_fn(c + 0x53d8) = 0;
    func_ov004_020ad79c(*inline_fn(c + 0xa8), (*inline_fn(c + 0xb4)) - 1);
  }
}
