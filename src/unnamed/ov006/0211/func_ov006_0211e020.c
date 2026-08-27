
void func_ov006_0211e020(char *c, int i)
{
  int n;
  char *p = c;
  for (n = 0; n < 0x10; n++)
  {
    if (((*((unsigned char *) (p + 0x4a70))) == 0) != 0)
    {
      char *d = (c + (n * 0x14)) + 0x4000;
      char *s = c + (i * 0x24);
      unsigned char *tbl = (unsigned char *) (c + 0x4680);
      *((unsigned char *) (((c + (n * 0x14)) + 0x4000) + 0xa70)) = 1;
      *((unsigned char *) (d + 0xa71)) = 0;
      *((int *) (((c + (n * 0x14)) + 0x4000) + 0xa60)) = *((int *) ((c + (i * 0x24)) + 0x4660));
      *((int *) (((c + (n * 0x14)) + 0x4000) + 0xa64)) = (*((int *) (s + 0x4664))) - 0x10000;
      *((int *) (((c + (n * 0x14)) + 0x4000) + 0xa6c)) = (tbl[i * 0x24] * 0x3c) + 0x10;
      *((unsigned char *) (((c + (n * 0x14)) + 0x4000) + 0xa73)) = 0;
      *((unsigned char *) (((c + (n * 0x14)) + 0x4000) + 0xa72)) = ((unsigned char *) (c + 0x4680))[i * 0x24];
      return;
    }
    p += 0x14;
  }

}
