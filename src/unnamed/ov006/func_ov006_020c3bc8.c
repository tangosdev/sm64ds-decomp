extern void func_020c3adc(void *);
void func_ov006_020c3bc8(void *a)
{
  int i = 0;
  char *p = (char *) a;
  for (; i < 22; i++)
  {
    *((int *) (p + 0x48)) = 0;
    p += 0x98;
  }
  func_020c3adc(a);
}
