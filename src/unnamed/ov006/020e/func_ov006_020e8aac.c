
#pragma opt_strength_reduction off
void func_ov006_020e8aac(char *c)
{
  int i;
  char *new_var;
  for (i = 0; i < 0x14; i++)
  {
    *((unsigned char *) ((c + (i * 0x20)) + 0x52d4)) = 0;
    *((unsigned char *) ((c + (i * 0x20)) + 0x52d8)) = 0;
  }

  if (1)
  {
  }
  for (i = 0; i < 0x14; i++)
  {
    *((unsigned char *) ((c + ((i * 0x20) & 0xFFFFFFFFFFFFFFFFu)) + 0x52d4)) = 1;
    *((unsigned char *) ((c + (i * 0x20)) + 0x52d9)) = 0;
    *((unsigned char *) ((c + (i * 0x20)) + 0x52da)) = 0;
    *((short *) ((c + (i * 0x20)) + 0x52cc)) = 0;
    new_var = (c + (i * 0x20)) + 0x52d2;
    *((short *) new_var) = ((i & 0xf) << 3) << 2;
  }

}
