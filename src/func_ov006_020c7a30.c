
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef short s16;
extern int _Z15ApproachLinear2Rsss(short *v, short t, short s);
extern void func_ov006_020c8270(char *c);
extern void func_ov006_020c85a0(char *c);
extern void func_ov006_020c833c(char *c);
extern void func_ov006_020c802c(char *c);
extern void func_ov006_020c7c68(char *c);
extern void func_ov006_020c79a8(char *c);
extern u8 data_020a0e40;
extern u8 data_020a0de8[][4];
extern u8 data_020a0de9[][4];
extern u8 data_020a0dea[][4];
extern u8 data_020a0deb[][4];
extern int data_ov006_0213b008;
void func_ov006_020c7a30(char *c)
{
  u32 idx = data_020a0e40;
  int flag = 0;
  if (data_020a0de8[idx][0] != 0)
  {
    if (data_020a0de9[idx][0] != 0)
    {
      flag = 1;
    }
  }
  if (flag != 0)
  {
    int b = flag = data_020a0deb[idx][0];
    int x = (*((s16 *) (c + 0x38))) - 0x20;
    int dz = (*((s16 *) (c + 0x36))) - data_020a0dea[idx][0];
    if (dz < 0)
    {
      dz = -dz;
    }
    if (dz < 0x18)
    {
      int dx = x - b;
      if (dx < 0)
      {
        dx = -dx;
      }
      if (dx < 0x26)
      {
        _Z15ApproachLinear2Rsss((short *) (c + 0x32), 0, 8);
      }
    }
  }
  {
    int v = *((int *) (c + 0x14));
    if (v < (-0x6c000))
    {
      v = -0x6c000;
    }
    else
      if (v > 0x6c000)
    {
      v = 0x6c000;
    }
    *((int *) (c + 0x14)) = v;
  }
  if ((*((u16 *) (c + 0x10))) == 1)
  {
    *((u16 *) (c + 0x10)) = 0;
    *((int *) (c + 0x24)) = data_ov006_0213b008;
    func_ov006_020c8270(c);
  }
  else
    if (_Z15ApproachLinear2Rsss((short *) (c + 0x32), 0, 1))
  {
    *((u16 *) (c + 0x10)) = 0;
    if ((*((int *) (c + 0x24))) > 0)
    {
      func_ov006_020c85a0(c);
      func_ov006_020c833c(c);
    }
    else
    {
      func_ov006_020c802c(c);
      func_ov006_020c7c68(c);
    }
  }
  else
  {
    if ((*((s16 *) (c + 0x38))) < 0xbc)
    {
      return;
    }
    func_ov006_020c79a8(c);
  }
}
