
extern int RandomIntInternal(int *seed);
extern int data_ov006_0213bd18[];
extern int RNG_STATE;
int func_ov006_020da8e4(void)
{
  int r5 = 0;
  int sum = 0;
  int i;
  for (i = 0; i < 6; i++)
  {
    sum += data_ov006_0213bd18[i];
  }

  sum = (sum * ((int) ((((unsigned int) RandomIntInternal(&RNG_STATE)) & 0x7fffffff) >> 0x13))) >> 0xc;
  for (i = 0; i < 6; i++)
  {
    sum -= data_ov006_0213bd18[i];
    if (sum < 0)
    {
      r5 = i & 0xff;
      data_ov006_0213bd18[i] -= 1;
      break;
    }
  }

  return r5;
}
