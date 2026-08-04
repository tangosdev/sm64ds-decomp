
extern int data_ov006_0212ee58[];
extern int data_ov006_0212ee4c[];
void func_ov006_0211a234(char *c, int i)
{
  int o = i * 0x24;
  char *bIdx = c + 0x51d2;
  char *bVal = c + 0x51b0;
  char *b50;
  unsigned char idx;
  *((unsigned char *) (((c + o) + 0x5000) + 0x1cd)) = 1;
  idx = *((unsigned char *) (bIdx + o));
  *((int *) (bVal + o)) = (*((int *) (bVal + o))) + data_ov006_0212ee58[idx];
  if ((i && i) != 0)
  {
  }
  *((int *) (((c + o) + 0x5000) + 0x1bc)) = -0x3200;
  *((int *) (((c + o) + 0x5000) + 0x1b8)) = data_ov006_0212ee4c[*((unsigned char *) (bIdx + o))];
  *((short *) ((c + o) + 0x51c8)) = 0;
  *((unsigned char *) (((c + o) + 0x5000) + 0x1d1)) = 1;
}
