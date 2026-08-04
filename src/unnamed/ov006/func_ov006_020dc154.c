
extern void func_ov006_020ddeb0(char *o);
void func_ov006_020dc154(char *base, int idx)
{
  unsigned short *p = (unsigned short *) ((base + 0x51ba) + (idx * 0x1c));
  if ((*p) != 0)
  {
    *p = (*p) - 1;
    return;
  }
  {
    char *o = (base + (idx * 0x1c)) + 0x5000;
    *((unsigned char *) (o + 0x1bf)) = 3;
    *((int *) (((base + (idx * 0x1c)) + 0x5000) + 0x1b4)) = 0;
    *((unsigned char *) (((base + (idx * 0x1c)) + 0x5000) + 0x1c0)) = 1;
    *((unsigned char *) (((base + (idx * 0x1c)) + 0x5000) + 0x1c1)) = 0;
  }
  func_ov006_020ddeb0(base);
}
