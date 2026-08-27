void func_ov006_020e83bc(char *c, int i)
{
  unsigned long new_var;
  char *b = c + (i * 32);
  char *q = c + 0x52bc;
  new_var = i;
  *((int *) (q + (new_var * 32))) = (*((int *) (q + (new_var * 32)))) + (*((int *) (b + 0x52c4)));
  *((int *) ((c + 0x52c0) + (new_var * 32))) = (*((int *) ((c + 0x52c0) + (new_var * 32)))) + (*((int *) (b + 0x52c8)));
  if ((*((int *) (b + 0x52c4))) > 0)
  {
    unsigned char e = *((unsigned char *) (b + 0x52d5));
    *((int *) ((c + 0x52c4) + (new_var * 32))) = (*((int *) ((c + 0x52c4) + (new_var * 32)))) - ((e * 16) + 0x10);
    if (((short) (*((int *) (b + 0x52c4)))) < 0)
    {
      *((int *) (b + 0x52c4)) = 0;
    }
  }
  else
    if ((*((int *) (b + 0x52c4))) < 0)
  {
    unsigned char e = *((unsigned char *) (b + 0x52d5));
    *((int *) ((c + 0x52c4) + (new_var * 32))) = (*((int *) ((c + 0x52c4) + (new_var * 32)))) + ((e * 16) + 0x10);
    if (((short) (*((int *) (b + 0x52c4)))) > 0)
    {
      *((int *) (b + 0x52c4)) = 0;
    }
  }
  else
  {
    *((unsigned char *) (b + 0x52da)) = 0;
  }
}
