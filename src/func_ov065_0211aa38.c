
extern short data_02082214[];
void func_ov065_0211aa38(char *r0, char *r1)
{
  unsigned short ang = *((unsigned short *) (r0 + 0x8e));
  int idx = ang >> 4;
  int v = (*((int *) (r0 + 0x38c))) << 2;
  int c = data_02082214[idx * 2];
  int res1 = (int) (((((long long) v) * c) + 0x800) >> 12);
  r1 = (char *)((long long)(int)(r1 + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
  int ip = *(int *)r1;
  *(int *)r1 = ip + res1;
  unsigned short ang2 = *((unsigned short *) (r0 + 0x8e));
  int idx2 = ang2 >> 4;
  int v2 = (*((int *) (r0 + 0x38c))) << 2;
  char *r0b = (char *)((long long)(int)(r1 + 8) & 0xFFFFFFFFFFFFFFFFLL);
  int s = data_02082214[(idx2 * 2) + 1];
  int lr = *(int *)r0b;
  int res2 = (int) (((((long long) v2) * s) + 0x800) >> 12);
  *(int *)r0b = lr + res2;
}
