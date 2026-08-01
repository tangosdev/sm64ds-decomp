
int func_ov007_020c5d5c(int *o)
{
  int n;
  int i;
  int sum = 0;
  for (i = 0; i < o[2]; i++)
  {
    int *p = ((int **) o[0xe])[i];
    unsigned short h = *((unsigned short *) (((char *) p) + 8));
    int *t = (int *) (((char *) p) + 0x20)[0];
    sum += ((int *) (*((int **) (((char *) p) + (0x20u)))))[h - 1];
  }

  return sum;
}
