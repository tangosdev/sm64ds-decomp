
extern unsigned char data_0209f250;
extern void func_02012790(int);
void func_0200d064(int param_1, unsigned int param_2)
{
  unsigned char tmp = data_0209f250;
  if (param_2 == tmp)
  {
    volatile int dummy;
    unsigned int *addr = (unsigned int *) ((param_1 + 0x154));
    unsigned int val = *addr;
    val &= ~2u;
    *addr = val;
    func_02012790(6);
  }
}
