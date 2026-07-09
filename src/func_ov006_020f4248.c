
typedef unsigned char u8;
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern void func_02012718(void *a, int b);
extern void func_ov004_020ad79c(int a, int b);
void func_ov006_020f4248(char *self, int idx)
{
  unsigned int s = *((u8 *) (self + 0x5338));
  unsigned int i;
  int ok;
  char *new_var;
  int n;
  int v;
  int w;
  if (s >= 2)
  {
    return;
  }
  i = data_020a0e40;
  ok = 0;
  if (data_020a0de8[data_020a0e40 * 4] != 0)
  {
    if (data_020a0de9[data_020a0e40 * 4] != 0)
    {
      ok = 1;
    }
  }
  if (ok == 0)
  {
    return;
  }
  n = idx * 0x18;
  v = data_020a0dea[i * 4] - ((*((int *) ((self + 0x51a8) + n))) >> 12);
  new_var = self + 0x51ac;
  w = data_020a0deb[i * 4] - ((*((int *) (new_var + n))) >> 12);
  if (v < (-0x10))
  {
    return;
  }
  if (v > 0x10)
  {
    return;
  }
  if (w < (-0x16))
  {
    return;
  }
  if (w > 0x16)
  {
    return;
  }
  *((u8 *) ((self + 0x532c) + s)) = *((u8 *) ((self + 0x51b8) + n));
  *((u8 *) ((self + 0x532e) + (*((u8 *) (self + 0x5338))))) = (u8) idx;
  {
    u8 *pc = (u8 *) (((long long) ((int) (self + 0x5338))) & 0xFFFFFFFFFFFFFFFFLL);
    *pc = (*pc) + 1;
  }
  *((u8 *) ((self + 0x51bc) + n)) = 3;
  func_02012718((void *) 0x143, *((int *) ((self + 0x51a8) + n)));
  if ((*((u8 *) (self + 0x533e))) != 0)
  {
    return;
  }
  {
    u8 *pd = (u8 *) (((long long) ((int) (self + 0x533e))) & 0xFFFFFFFFFFFFFFFFLL);
    *pd = (*pd) + 1;
  }
  func_ov004_020ad79c(*((int *) (self + 0xa8)), *((int *) (self + 0xb4)));
}
