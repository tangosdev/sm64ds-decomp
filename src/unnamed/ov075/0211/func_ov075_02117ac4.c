
extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
extern int data_ov075_0211d71c[];
extern void func_ov075_02116028(int *p, int v);
extern void func_020338b0(int a, int b, int c, int d);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(void *p, unsigned short a, unsigned short b, short c);
void func_ov075_02117ac4(char *r4)
{
  data_0209d45c = data_0209d45c & (~0xe);
  data_0209d454 = data_0209d454 & (~1);
  func_ov075_02116028(data_ov075_0211d71c, 6);
  func_020338b0(0x145, 0xb0, 0x17, -1);
  data_0209d454 = data_0209d454 | 5;
  *((unsigned char *) (r4 + 0x280)) = 0;
  _ZN3G2x18SetBlendBrightnessEPVtts((void *) 0x4000050, 0x1f, (double) (~5), 0);
  *((int *) (r4 + 0x264)) = 2;
}
