extern int __aeabi_uidiv(unsigned int a, unsigned int b);
extern struct LL
{
  int lo;
  int hi;
} data_020a5578[];
extern int data_020a5538[];
typedef void (*FP)(void *, int, int *, int, int, int);
void func_0204fce8(int *thiz, void *arg)
{
  int new_var;
  int i;
  int *pcount;
  unsigned char *p = (unsigned char *) thiz;
  int q = __aeabi_uidiv(thiz[4], thiz[5]);
  int scaled = q * thiz[8];
  for (i = 0; i < thiz[0xc]; i++)
  {
    new_var = data_020a5578[(p + i)[0x34]].lo;
    data_020a5538[i] = new_var + scaled;
  }

  ((FP) thiz[6])(arg, thiz[0xc], data_020a5538, q, thiz[2], thiz[7]);
  pcount = (int *) (((int) thiz + 0x20) & 0xFFFFFFFFFFFFFFFF);
  (*pcount)++;
  if (thiz[8] >= thiz[5])
  {
    thiz[8] = 0;
  }
}
