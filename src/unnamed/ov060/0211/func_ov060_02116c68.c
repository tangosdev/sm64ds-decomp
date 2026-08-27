extern int RandomIntInternal(int *seed);
extern int data_0209e650;
extern int data_ov060_0211934c[];

void func_ov060_02116c68(char *c)
{
  unsigned int r;
  *(short *)(c + 0x94) = (short)(((unsigned int)RandomIntInternal(&data_0209e650)) >> 0x10);
  if (*(unsigned char *)(c + 0x378) != 0)
  {
    r = ((unsigned int)RandomIntInternal(&data_0209e650)) >> 0x10;
    *(int *)(c + 0x98) = (int)((r % 10) << 0xc) >> 1;
  }
  else
  {
    r = ((unsigned int)RandomIntInternal(&data_0209e650)) >> 0x10;
    *(int *)(c + 0x98) = (int)((r % 10) * 0x7000) >> 1;
  }

  r = ((unsigned int)RandomIntInternal(&data_0209e650)) >> 0x10;
  *(int *)(c + 0xa8) = (r % 10) << 0xc;
  *(int *)(c + 0x9c) = -0x1000;

  *(short *)(c + 0x376) = (short)(((unsigned int)RandomIntInternal(&data_0209e650)) >> 0x10);
  *(int *)(c + 0xa0) = data_ov060_0211934c[*(unsigned char *)(c + 0x378)];
  *(int *)(c + 0x360) = 0x2000;
  *(int *)(c + 0x368) = 0x10;
}
