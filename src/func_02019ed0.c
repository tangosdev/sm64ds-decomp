#include "types.h"
extern u8 data_0209d4e4;
extern void func_0205f77c(int a, int b, int c);
void func_02019ed0(void){
  data_0209d4e4 = 1;
  func_0205f77c(0xc, 0, 0);
  data_0209d4e4 = 0;
}
