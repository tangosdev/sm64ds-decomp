
void func_ov006_0211dce0(char *base, int i)
{
  char *p = base + (i << 5);
  *((base + (i << 5)) + 0x4bba) = 1;
  *((short *) ((base + (i << 5)) + 0x4bb4)) = 0x40;
  *((int *) (p + 0x4ba8)) = -0xb00;
}
