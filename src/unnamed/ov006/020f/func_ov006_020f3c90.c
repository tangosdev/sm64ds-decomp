
void func_ov004_020ad79c(int r0arg, int r1arg);
inline int *inline_fn(char *arg0)
{
  return (int *) arg0;
}

void func_ov006_020f3c90(char *c)
{
  if ((*((unsigned char *) (c + 0x5337))) >= (*((unsigned short *) (c + 0x532a))))
  {
    *((short *) (c + 0x5324)) = 0x40;
    *inline_fn(c + 0x5314) = 3;
    *inline_fn(c + 0x5318) = 0;
    func_ov004_020ad79c((*inline_fn(c + 0xa8)) + ((3 - (*((unsigned char *) (c + 0x533a)))) * 6), (*inline_fn(c + 0xb4)) + 1);
  }
  if ((*((unsigned char *) (c + 0x533a))) >= (*((unsigned char *) (c + 0x533b))))
  {
    *((short *) (c + 0x5324)) = 0x40;
    *inline_fn(c + 0x5314) = 3;
    *inline_fn(c + 0x5318) = 0;
    func_ov004_020ad79c(*inline_fn(c + 0xa8), (*inline_fn(c + 0xb4)) - 1);
  }
}
