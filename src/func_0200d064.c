
extern unsigned char CURR_PLAYER_ID;
extern void func_02012790(int);
void func_0200d064(int param_1, unsigned int param_2)
{
  unsigned char tmp = CURR_PLAYER_ID;
  if (param_2 == tmp)
  {
    volatile int dummy;
    unsigned int *addr = (unsigned int *) ((param_1 + 0x154) & 0xFFFFFFFFFFFFFFFF);
    unsigned int val = *addr;
    val &= ~2u;
    *addr = val;
    func_02012790(6);
  }
}
