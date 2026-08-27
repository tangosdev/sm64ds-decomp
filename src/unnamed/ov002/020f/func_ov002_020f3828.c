
extern void SetSubBg1Offset(int a, int b);
extern void SetSubBg2Offset(int a, int b);
extern unsigned char data_0209d454;
void func_ov002_020f3828(int *c)
{
  c[0x140 / 4] = 0x80000;
  c[0x144 / 4] = 0x66000;
  c[0x148 / 4] = 0xfc000;
  c[0x14c / 4] = 0xbc000;
  *((volatile unsigned int *) 0x4001000) = ((*((volatile unsigned int *) 0x4001000)) & (~0xe000)) | 0x4000;
  *((volatile unsigned short *) 0x4001048) = (unsigned short) (((((*((volatile unsigned short *) 0x4001048)) & (~0x3f00)) | 0x1700) & 0xFFFFFFFFFFFFFFFFull) | 0x2000);
  *((volatile unsigned short *) 0x400104a) = (unsigned short) (((((*((volatile unsigned short *) 0x400104a)) & (~0x3f)) | 1) & 0xFFFFFFFFFFFFFFFFull) | 0x20);
  {
    int a = c[0x140 / 4] >> 12;
    int d = c[0x148 / 4] >> 12;
    int e = c[0x14c / 4] >> 12;
    int b = c[0x144 / 4] >> 12;
    *((volatile unsigned short *) 0x4001042) = (unsigned short) (((a << 8) & 0xff00) | (d & 0xff));
    *((volatile unsigned short *) 0x4001046) = (unsigned short) (((b << 8) & 0xff00) | (e & 0xff));
  }
  c[0x130 / 4] = -0x60000;
  c[0x134 / 4] = -0x38000;
  SetSubBg1Offset(c[0x130 / 4] >> 12, c[0x134 / 4] >> 12);
  SetSubBg2Offset(c[0x130 / 4] >> 12, c[0x134 / 4] >> 12);
  data_0209d454 |= 6;
}
