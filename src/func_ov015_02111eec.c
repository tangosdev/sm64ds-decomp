extern void Math_Function_0203b0fc(int *p, int target, int scale, int max);
extern void func_ov015_02111fb8(void *self, int idx);
void func_ov015_02111eec(char *c)
{
  Math_Function_0203b0fc((int *)(c + 0x5c), *(int *)(c + 0x320) - 0x168000, 0x800, 0x46000);
  {
    int *p = (int *)(((int)c + 0x334) & 0xFFFFFFFFFFFFFFFF);
    *p = *p - 1;
  }
  if (*(int *)(c + 0x334) > 0)
    return;
  *(int *)(c + 0x5c) = *(int *)(c + 0x320) - 0x168000;
  func_ov015_02111fb8(c, 2);
}
