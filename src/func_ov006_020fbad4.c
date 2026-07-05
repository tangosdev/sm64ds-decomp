
void func_ov006_020fbad4(char *c)
{
  int i;
  for (i = 0; i < 0x1e; i++)
  {
    char *r2 = c + 0x4c00;
    unsigned short v = *((unsigned short *) (r2 + 0xfa));
    if (v != 0)
    {
      unsigned short *p = (unsigned short *) (((long long) ((int) (c + 0x4cfa))) & 0xFFFFFFFFFFFFFFFFLL);
      *p = (*p) - 1;
      if ((*((short *) ((c + 0x4c00) + 0xfa))) < 0)
      {
        *((short *) ((c + 0x4c00) + 0xfa)) = 0;
      }
    }
    c += 0xc;
  }

}
