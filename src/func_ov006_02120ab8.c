
int _Z15ApproachLinear2Rsss(short *a, short b, short c);
void func_ov006_02120b7c(void *node);
int _Z14ApproachLinearRiii(int *a, int b, int c);
void func_0203d630(int *p, int m);
void func_ov006_02120ab8(char *this)
{
  if ((*((short *) (this + 0x20))) == 0)
  {
    return;
  }
  if (_Z15ApproachLinear2Rsss((short *) (this + 0x1c), 0, 1))
  {
    func_ov006_02120b7c(this);
    *((short *) (this + 0x20)) = 0;
  }
  {
    int *p = (int *) (((int) this + 4) & 0xFFFFFFFFFFFFFFFF);
    *p = *p + (*((int *) (this + 0x14)));
  }
  _Z14ApproachLinearRiii((int *) (this + 8), *((int *) (this + 0x10)), 0x1800);
  func_0203d630((int *) (this + 0x14), 0xf00);
}
