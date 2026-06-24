
extern int *func_020beb68;
void func_ov006_020f593c(int this)
{
  int n;
  int i;
  if (func_020beb68 != 0)
  {
    n = *((int *) (((char *) func_020beb68) + 0xa8));
  }
  else
  {
    n = 0;
  }
  if (n >= 5)
  {
    n = 5;
  }
  i = 0;
  *((unsigned char *) ((this + 0x5000) + 0x409)) = (unsigned char) n;
  if (n <= 0)
  {
    return;
  }
  do
  {
    *((unsigned char *) (((this + (i * 0x14)) + 0x5000) + 0x398)) = 1;
    *((unsigned char *) (((this + (i * 0x14)) + 0x5000) + 0x39a)) = 0;
    *((short *) (((this + (i * 0x14)) + 0x5300) + 0x96)) = i * 0x10;
    i++;
  }
  while (i < n);
}
