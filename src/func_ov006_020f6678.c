
extern void func_ov006_020f593c(void *);
void func_ov006_020f6678(char *c)
{
  char *q = c + 0x5300;
  char *h;
  if ((*((unsigned short *) (q + 0xe2))) != 0)
  {
    h = (char *) ((((int) c) + 0x53e2) & 0xFFFFFFFFFFFFFFFF);
    *((unsigned short *) h) = (unsigned short) ((*((unsigned short *) h)) - 1);
    if ((*((short *) ((c + 0x5300) + 0xe2))) > 0)
    {
      return;
    }
    func_ov006_020f593c(c);
    return;
  }
  {
    long i;
    char *p;
    for (i = 0; i < 0x14; i++)
    {
      p = c;
      *((unsigned char *) (((i * 0x18) + p) + 0x51bc)) = 2;
    }

  }
  *((int *) (c + 0x53d8)) = 0;
  *((int *) (c + 0x53d4)) = 2;
}
