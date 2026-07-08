extern int func_0203daac(void);
extern unsigned char TOUCH_INPUT_ARR[];
void func_0203bb14(void){
  unsigned char* p = TOUCH_INPUT_ARR;
  int i = 0;
  if (func_0203daac() <= 0) return;
  do {
    p[0] = 0;
    p[1] = 0;
    i++;
    p += 4;
  } while (i < func_0203daac());
}
