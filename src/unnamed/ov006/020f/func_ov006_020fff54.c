
void func_ov006_020fff54(char *base, int i)
{
  char *p;
  *((base + (i << 5)) + 0x5637) = 0x10;
  *((unsigned char *) ((base + (i << 5)) + 0x5638)) = 0;
  *((short *) ((base + (i << 5)) + 0x5630)) = 0;
  *((unsigned char *) ((base + (i << 5)) + 0x5639)) = 1;
  *((unsigned char *) ((base + (i << 5)) + 0x5635)) = 1;
}
