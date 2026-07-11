
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
void func_ov006_020fb670(char *obj)
{
  int cnt;
  int i;
  unsigned int r;
  char *p;
  if ((*((unsigned char *) ((obj + 0x5000) + 0xc34))) != 0)
  {
    return;
  }
  *((unsigned char *) ((obj + 0x5000) + 0xbc6)) = 1;
  *((unsigned char *) ((obj + 0x5000) + 0xbc8)) = 0;
  *((int *) ((obj + 0x5000) + 0xbc0)) = 0;
  r = (((unsigned int) RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
  *((short *) ((obj + 0x5b00) + 0xc4)) = (short) ((((r << 5) >> 15) << 3) + 0x200);
  cnt = 0;
  for (i = 0, p = obj; i < 4; i++)
  {
    if ((*((unsigned char *) ((p + 0x5000) + 0xbd4))) != 0)
    {
      cnt++;
    }
    p += 0xc;
  }

  if (cnt == 1)
  {
    r = (((unsigned int) RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff;
    *((short *) ((obj + 0x5b00) + 0xc4)) = (short) ((((r << 5) >> 15) << 2) + 0x200);
  }
  unsigned char *bp = (unsigned char *) (((int) obj + 0x5c34) & 0xFFFFFFFFFFFFFFFF);
  *bp = *bp + 1;
}
