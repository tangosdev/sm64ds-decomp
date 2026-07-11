
extern int data_ov006_0212e948[];
void func_ov006_020f6a00(char *this)
{
  int idx;

  if ((*((short *) ((this + 0x5300) + 0xe6))) < 4)
  {
    return;
  }
  idx = ((((unsigned short) (*((unsigned short *) ((this + 0x5300) + 0xea)))) << 1) - 1) - (*((short *) ((this + 0x5300) + 0xe8)));
  *((unsigned char *) ((this + (idx * 0x18)) + 0x51bb)) = 1;
  {
    short *sp = (short *) (((int) this + 0x53e8) & 0xFFFFFFFFFFFFFFFF);
    *sp = (*sp) + 1;
  }
  this = this + 0x5000;
  if ((*((short *) ((this - 0x5000 + 0x5300) + 0xe8))) >= data_ov006_0212e948[*((unsigned char *) (this + 0x40a))])
  {
    *((int *) (this + 0x3d8)) = 5;
  }
}
