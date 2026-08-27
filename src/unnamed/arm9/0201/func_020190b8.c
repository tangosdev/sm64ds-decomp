extern int func_02019018(void);
extern unsigned char data_0209d464;

void func_020190b8(void){
  if (func_02019018() == 0) return;
  *(volatile int*)0x4000540 = 3;
  data_0209d464 = 1;
}
