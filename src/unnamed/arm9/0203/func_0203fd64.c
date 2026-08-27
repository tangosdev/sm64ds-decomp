extern unsigned short data_020a0f2c;
extern void func_02040634(void);
extern int data_020a0f94;
extern int func_0206259c(void* fn, int arg);
int func_0203fd64(void){
  if (func_0206259c((void*)func_02040634, data_020a0f2c) != 2){
    data_020a0f94 = 1;
    return 0;
  }
  return 1;
}
