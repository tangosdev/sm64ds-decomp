
void func_ov006_02104bb0(char *c)
{
  char *base;
  unsigned short *p = (unsigned short *) (((long long) ((int) (c + 0x4682))));
  *p = (*p) + 1;
  if ((*((unsigned short *) ((c + 0x4600) + 0x82))) < 8)
  {
    return;
  }
  *((unsigned short *) ((c + 0x4600) + 0x82)) = 0;
  {
    int *q = (int *) (((long long) ((int) (c + 0x467c))));
    *q = (*q) + 0x4000;
  }
  *((unsigned char *) (c + 0x4686)) = 2;
}
