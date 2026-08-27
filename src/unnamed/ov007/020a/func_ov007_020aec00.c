extern int *data_ov007_0210342c;
void func_ov007_020aec00(void)
{
  int *p28;
  int zero;
  int neg1;
  int i;
  int sum;
  int k;
  int *g;
  int off;
  unsigned char *arr;
  unsigned char *e;
  unsigned char *row;
  i = 0;
  g = data_ov007_0210342c;
  off = i;
  p28 = (int *) g[10];
  zero = i;
  neg1 = -1;
  for (; i < 3; i++)
  {
    arr = (unsigned char *) (*p28);
    sum = zero;
    row = arr + i;
    if (row[0xb] != 0)
    {
      k = zero;
      e = arr + off;
      while (k < 15)
      {
        sum += e[0x11];
        sum += e[0x6b];
        e++;
        k++;
      }
      sum += row[0x98];
    }
    else
      sum = neg1;
    ((short *) (((char *) p28) + 4))[i] = (short) sum;
    off += 15;
  }
}
