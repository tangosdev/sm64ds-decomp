
void func_ov002_020f2d70(char *c, int i)
{
  char *p;
  p = c + 0x1c4;
  *((short *) (p + (i * 8))) = (*((unsigned short *) (p + (i * 8)))) + 1;
  p = c + (i * 8);
  if ((*((unsigned short *) (p + 0x1c4))) < 0x1e)
  {
    return;
  }
  *((short *) (p + 0x1c4)) = 0;
  *((unsigned char *) ((c + 0x1c9) + (i * 8))) = (*((unsigned char *) ((c + 0x1c9) + (i * 8)))) + 1;
}
