
extern void func_ov060_021150d0(void *a);
extern void func_ov060_021150c4(void *p);
void func_ov060_02115060(char *c)
{
  if (*((unsigned char *) (c + 0x415)) == 0)
  {
    if (*((unsigned char *) (c + 0x416)) == 0)
    {
      func_ov060_021150d0(c);
    }
    else
    {
      func_ov060_021150c4(c);
    }
    (*((unsigned char *) ((((unsigned int) c) + 0x415) & 0xFFFFFFFFFFFFFFFF)))++;
    return;
  }
  *((unsigned char *) (c + 0x415)) = 0;
  *((int *) (c + 0x40c)) = 0xe;
}
