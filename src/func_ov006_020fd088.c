
typedef unsigned short u16;
extern void func_ov006_020fdaf0(char *, int);
extern void func_ov006_020fca1c(char *, int);
extern void func_ov006_020fc9b0(char *, int);
void func_ov006_020fd088(char *self, int idx)
{
  char *new_var;
  *((int *) ((self + 0x4660) + (idx * 0x38))) = (*((int *) ((self + 0x4660) + (idx * 0x38)))) + (*((int *) ((self + (idx * 0x38)) + 0x4668)));
  new_var = self;
  *((int *) ((new_var + 0x4664) + (idx * 0x38))) = (*((int *) ((new_var + 0x4664) + (idx * 0x38)))) + (*((int *) ((self + (idx * 0x38)) + 0x466c)));
  {
    int raw = *((u16 *) ((new_var + ((idx * 0x38) & 0xFFFFFFFF)) + 0x4686));
    u16 ip = (u16) (raw - 0x4000);
    short tgt = (short) ip;
    short *cur = (short *) ((self + 0x4684) + (idx * 0x38));
    if ((*cur) > tgt)
    {
      *cur = (*cur) - 0x100;
      if ((*cur) <= ((short) ip))
      {
        *cur = ip & 0xFFFFFFFFFFFFFFFF;
      }
    }
    else
      if ((*cur) < ((short) ip))
    {
      *cur = (*cur) + 0x100;
      raw = ip & 0xFFFFFFFFFFFFFFFF;
      if ((*cur) >= ((short) ip))
      {
        *cur = raw;
      }
    }
  }
  func_ov006_020fdaf0(new_var, idx);
  func_ov006_020fca1c(self, idx);
  func_ov006_020fc9b0(self, idx);
}
