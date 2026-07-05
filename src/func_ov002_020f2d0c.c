
void func_ov002_020f2d0c(char *c, int i)
{
  long new_var;
  *((short *) ((c + 0x1c4) + (i * 8))) = (*((unsigned short *) ((c + 0x1c4) + (i * 8)))) + 1;
  new_var = 8;
  char *r = c + (i * new_var);
  char *b = r + 0x100;
  if ((*((unsigned short *) (b + 0xc4))) < 0x39)
  {
    return;
  }
  *((short *) ((r + 0x100) + 0xc4)) = 0;
  *((unsigned char *) ((c + 0x1c9) + (i * new_var))) = (*((unsigned char *) ((c + 0x1c9) + (i * new_var)))) + 1;
}
