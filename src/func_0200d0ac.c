
extern unsigned char CURR_PLAYER_ID;
extern void func_02012790(int);
int func_0200d0ac(int a, unsigned int b)
{
  if (b == CURR_PLAYER_ID)
  {
    unsigned short v = *((unsigned short *) (((unsigned char *) (*((unsigned int **) (a + 0x140)))) + 0x26));
    if (v & 1)
    {
      return 0;
    }
    *((unsigned int *) ((a + 0x154) & 0xFFFFFFFFFFFFFFFFu)) |= 2u;
    func_02012790(7);
  }
  return 1;
}
