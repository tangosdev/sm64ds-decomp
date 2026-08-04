
extern int data_ov006_0212ef18[];
extern int data_ov006_0212ef24[];
void func_ov006_0211aed0(char *c, int i)
{
  int new_var;
  int o = i * 0x24;
  char *bIdx = c + 0x51d2;
  char *bVal = c + 0x51b0;
  char *b50 = (c + o) + 0x5000;
  unsigned char idx;
  *((unsigned char *) (b50 + 0x1cd)) = 1;
  idx = *((unsigned char *) (bIdx + o));
  new_var = (*((int *) (bVal + o))) + data_ov006_0212ef18[idx];
  *((int *) (bVal + o)) = *((int *) (bVal + o));
  if (((!o) && (!o)) && (!o))
  {
  }
  *((int *) (bVal + o)) = new_var;
  *((int *) (((c + o) + 0x5000) + 0x1bc)) = -0x4800;
  *((int *) (((c + o) + 0x5000) + 0x1b8)) = data_ov006_0212ef24[*((unsigned char *) (bIdx + o))];
  *((short *) ((c + o) + 0x51c8)) = 0;
  *((unsigned char *) (((c + o) + 0x5000) + 0x1d1)) = 1;
}
