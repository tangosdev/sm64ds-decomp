extern void func_020190b8(void);
extern unsigned char data_0208ee00;

void func_02019028(void){
  func_020190b8();
  *(volatile unsigned short*)0x4000304 &= ~8;
  *(volatile unsigned short*)0x4000304 &= ~4;
  data_0208ee00 = 0;
}
