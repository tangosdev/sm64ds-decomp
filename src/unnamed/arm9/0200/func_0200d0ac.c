
extern unsigned char data_0209f250;
extern void func_02012790(int);
int func_0200d0ac(int a, unsigned int b)
{
  if (b == data_0209f250)
  {
    unsigned short v = *((unsigned short *) (((unsigned char *) (*((unsigned int **) (a + 0x140)))) + 0x26));
    if (v & 1)
    {
      return 0;
    }
    *((unsigned int *) (a + 0x154)) |= 2u;
    func_02012790(7);
  }
  return 1;
}
