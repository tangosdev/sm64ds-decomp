
extern int data_ov006_0212ee64[];
extern int data_ov006_0212ef00[];
void func_ov006_0211a420(char *c, int i)
{
  int o = i * 0x24;
  unsigned char idx;
  char *bIdx = c + 0x51d2;
  char *bVal;
  char *b50 = (c + o) + 0x5000;
  *((unsigned char *) (b50 + 0x1cd)) = 1;
  bVal = c + 0x51b0;
  idx = *((unsigned char *) (bIdx + o));
  *((int *) (bVal + o)) = (*((int *) (bVal + o))) + data_ov006_0212ee64[idx];
  if ((o && o) && o)
  {
  }
  if (1)
  {
    *((int *) (((c + o) + 0x5000) + 0x1bc)) = -0x3800;
    *((int *) (((c + o) + 0x5000) + 0x1b8)) = data_ov006_0212ef00[*((unsigned char *) (bIdx + o))];
    *((short *) (0x51c8 + (c + o))) = 0;
    *((unsigned char *) (((c + o) + 0x5000) + 0x1d1)) = 1;
  }
}
