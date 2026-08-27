
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
void func_ov006_0210fa6c(char *c)
{
  int i;
  unsigned char z;
  int *seed;
  if ((*((unsigned char *) (c + 0x73))) != 1)
  {
    return;
  }
  if ((*((unsigned char *) (c + 0x74))) != 0)
  {
    return;
  }
  *((unsigned char *) (c + 0x75)) = 1;
  z = 0;
  for (i = 0; i < 3; i++)
  {
    *((unsigned char *) ((c + i) + 0x70)) = z;
    ((int *) (c + 0x64))[i] = ((((((unsigned) RandomIntInternal(&data_0209d4b8)) >> 16) & 0x7fff) * 3) >> 15) + 3;
  }

}
