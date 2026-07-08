
extern unsigned char TOP_SCREEN_RELATED;
extern unsigned char BOTTOM_SCREEN_RELATED;
extern int data_ov075_0211d71c[];
extern void func_ov075_02116028(int *p, int v);
extern void func_020338b0(int a, int b, int c, int d);
extern void _ZN3G2x18SetBlendBrightnessEPVtts(void *p, unsigned short a, unsigned short b, short c);
void func_ov075_02117ac4(char *r4)
{
  TOP_SCREEN_RELATED = TOP_SCREEN_RELATED & (~0xe);
  BOTTOM_SCREEN_RELATED = BOTTOM_SCREEN_RELATED & (~1);
  func_ov075_02116028(data_ov075_0211d71c, 6);
  func_020338b0(0x145, 0xb0, 0x17, -1);
  BOTTOM_SCREEN_RELATED = BOTTOM_SCREEN_RELATED | 5;
  *((unsigned char *) (r4 + 0x280)) = 0;
  _ZN3G2x18SetBlendBrightnessEPVtts((void *) 0x4000050, 0x1f, (double) (~5), 0);
  *((int *) (r4 + 0x264)) = 2;
}
