
extern int data_ov006_0212e918[];
void func_ov006_020f6b78(char *this)
{
  int idx;
  char *r3;
  if ((*((short *) ((this + 0x5300) + 0xe6))) < 1)
  {
    return;
  }
  idx = ((((unsigned short) (*((unsigned short *) ((this + 0x5300) + 0xea)))) << 1) - 1) - (*((short *) ((this + 0x5300) + 0xe8)));
  *((unsigned char *) ((this + (idx * 0x18)) + 0x51bb)) = 1;
  if ((this && this) && this)
  {
  }
  {
    short *p = (short *) (((int) (this + 0x53e8)) & 0xFFFFFFFFFFFFFFFF);
    *p = (*p) + 1;
  }
  r3 = this + 0x5300;
  if ((*((short *) (r3 + 0xe8))) >= data_ov006_0212e918[*((unsigned char *) (this + 0x540a))])
  {
    *((int *) (this + 0x53d8)) = 2;
  }
}
