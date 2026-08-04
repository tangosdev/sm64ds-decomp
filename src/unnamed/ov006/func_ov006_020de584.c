extern void func_020ddd6c(void *);
void func_ov006_020de584(void *a)
{
  char *p = (char *) a;
  if ((*((unsigned char *) (p + 0xc4))) == 0)
  {
    *((unsigned char *) (p + 0xc3)) = 1;
    *((unsigned char *) (p + 0xc4)) = 1;
    *((short *) (p + 0xc0)) = 0;
  }
  func_020ddd6c(a);
}
