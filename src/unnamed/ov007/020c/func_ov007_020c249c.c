
struct S
{
  char pad8[8];
  unsigned short n;
  char pad[14];
  int *arr;
};
int func_ov007_020c249c(struct S *s)
{
  int i;
  int sum = 0;
  unsigned short n = s->n;
  for (i = 0; i < n; i++)
  {
    sum = sum + s->arr[i];
  }

  return sum;
}
