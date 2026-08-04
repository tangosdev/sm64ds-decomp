
extern unsigned char data_ov006_0212e308[];
extern unsigned char data_ov006_0212e30c[];
void func_ov006_020dc4c8(char *c, int i)
{
  int n = i * 0x14;
  unsigned short cnt = *((unsigned short *) ((c + 0x519e) + n));
  int new_var;
  *((unsigned short *) ((c + 0x519e) + n)) = cnt + 1;
  unsigned char idx = *((unsigned char *) ((c + 0x51a4) + n));
  if ((*((unsigned short *) ((c + 0x519e) + n))) >= data_ov006_0212e308[idx])
  {
    *((unsigned short *) ((c + 0x519e) + n)) = 0;
    *((unsigned char *) ((c + 0x51a4) + n)) = (*((unsigned char *) ((c + 0x51a4) + n))) + 1;
    *((unsigned char *) ((c + 0x51a4) + n)) = (*((unsigned char *) ((c + 0x51a4) + n))) & 1;
  }
  *((unsigned char *) ((c + n) + 0x51a3)) = data_ov006_0212e30c[*((unsigned char *) ((c + 0x51a4) + n))];
  if ((*((int *) (c + 0x51c8))) == 5)
  {
    return;
  }
  unsigned short w = *((unsigned short *) ((c + 0x519c) + n));
  if (w != 0)
  {
    *((unsigned short *) ((c + 0x519c) + n)) = (new_var = w - 1);
    return;
  }
  *((c + n) + 0x51a1) = 3;
  *((unsigned char *) ((c + 0x51a4) + n)) = 0;
  *((unsigned short *) ((c + 0x519e) + n)) = 0;
  *((unsigned short *) ((c + 0x519c) + n)) = 0;
}
