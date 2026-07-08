
typedef short s16;
typedef unsigned char u8;
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int);
extern int data_ov006_021428c8;
extern u8 data_020a0e40;
extern u8 TOUCH_INPUT_ARR[];
extern u8 data_020a0de9[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
int func_ov006_02108e24(int *p)
{
  int idx;
  int flag;
  int new_var;
  int dx;
  int dz;
  new_var = 4;
  if (data_ov006_021428c8 != 0)
  {
    return 0;
  }
  idx = data_020a0e40;
  flag = 0;
  if (TOUCH_INPUT_ARR[idx * 4])
  {
    if (data_020a0de9[idx * 4] != 0)
    {
      flag = 1;
    }
  }
  if (flag == 0)
  {
    return 0;
  }
  idx = data_020a0e40;
  dx = (p[0] >> 0xc) - data_020a0dea[idx * new_var];
  dz = (p[1] >> 0xc) - data_020a0deb[idx * 4];
  if ((((dx >= (-8)) && (dx < 8)) && (dz >= (-8))) && (dz < 8))
  {
    _ZN5Sound12PlayBank2_2DEj(0x15d);
    data_ov006_021428c8 = (int) p;
    *((u8 *) (((char *) p) + 0x32)) = 2;
    p[2] = dx << 0xc;
    p[3] = dz << 0xc;
    return 1;
  }
  return 0;
}
