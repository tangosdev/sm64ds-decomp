
typedef int s32;
extern int data_ov006_02141fe4[];
extern void *data_ov006_0213ca9c[];
extern char data_ov006_02137cd8[];
extern s32 func_ov004_020ad674(void);
extern void func_ov004_020af948(void *a, int b, int c, int d);
void func_ov006_020ea350(void)
{
  int x;
  int y;
  s32 r;
  int *new_var;
  new_var = data_ov006_02141fe4;
  if ((*(((unsigned char *) data_ov006_02141fe4) + 0x10)) != 1)
  {
    return;
  }
  x = data_ov006_02141fe4[0] >> 12;
  y = new_var[1] >> 12;
  r = func_ov004_020ad674();
  func_ov004_020af948(*((void **) (((char *) data_ov006_0213ca9c[r]) + 0x34)), x - 8, y, 0);
  func_ov004_020af948(*((void **) (data_ov006_02137cd8 + 0x64)), x + 8, y, 0);
}
