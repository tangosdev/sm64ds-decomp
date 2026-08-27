extern unsigned short data_02099fcc;
extern unsigned short data_020a6084;

void func_02053b98(void){
  unsigned v = *(volatile unsigned*)0x4000000;
  data_02099fcc = 0;
  data_020a6084 = (unsigned short)((v & 0x30000) >> 0x10);
  *(volatile unsigned*)0x4000000 = v & ~0x30000;
}
