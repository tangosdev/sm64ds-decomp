
void func_ov006_020d5f2c(char *c, int i)
{
  char *new_var2;
  char *ip;
  unsigned char new_var;
  new_var2 = c + (i * 0x10);
  if (((unsigned char) (*((unsigned char *) ((new_var2 + 0x6000) + 0x2ae)))) >= 4)
  {
    *((unsigned char *) ((new_var2 + 0x6000) + 0x2ae)) = 4;
    *((unsigned char *) ((new_var2 + 0x6000) + 0x2ac)) = 2;
    return;
  }
  {
    unsigned short *p = (unsigned short *) ((c + 0x62a8) + (i * 0x10));
    *p = (*p) + 1;
  }
  if ((*((unsigned short *) (new_var2 + 0x62a8))) < 3)
  {
    return;
  }
  *((unsigned short *) (new_var2 + 0x62a8)) = 0;
  ip = new_var2 + 0x6000;
  {
    unsigned char *q = (unsigned char *) ((c + 0x62ae) + (i * 0x10));
    *q = (*q) + 1;
  }
  new_var = (unsigned char) (*((unsigned char *) (ip + 0x2ae)));
  if (new_var >= 4)
  {
    *((unsigned char *) ((new_var2 + 0x6000) + 0x2ae)) = 4;
    *((unsigned char *) (ip + 0x2ac)) = 2;
  }
}
