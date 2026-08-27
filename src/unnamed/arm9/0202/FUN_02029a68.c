extern int data_0209f324;
extern int data_0209d4b0;
extern unsigned char data_0209f274;

void FUN_02029a68(void){
  if (data_0209d4b0 == data_0209f324 + 0x240) {
    data_0209d4b0 = 0;
    *(volatile unsigned short*)0x4000050 = 0;
  }
  data_0209f274 = 0;
}
