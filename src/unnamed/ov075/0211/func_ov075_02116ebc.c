extern unsigned char data_0209d45c;
extern unsigned char data_0209d454;
extern int data_ov075_0211d71c[];
extern void func_ov075_02116028(void* p, int v);
extern void func_020338b0(int a, int b, int c, int d);
extern void func_02020334(void);
extern void func_02012790(int x);
void func_ov075_02116ebc(char* c){
  data_0209d45c &= ~4;
  data_0209d454 &= ~1;
  func_ov075_02116028(data_ov075_0211d71c, 6);
  func_020338b0(0x145, 0xb0, 0x1d, -1);
  data_0209d454 |= 5;
  *(unsigned char*)(c+0x280) = 0;
  func_02020334();
  func_02012790(0x120);
}
