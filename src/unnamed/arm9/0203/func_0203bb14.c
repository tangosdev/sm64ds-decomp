extern int func_0203daac(void);
extern unsigned char data_020a0de8[];
void func_0203bb14(void){
  unsigned char* p = data_020a0de8;
  int i = 0;
  if (func_0203daac() <= 0) return;
  do {
    p[0] = 0;
    p[1] = 0;
    i++;
    p += 4;
  } while (i < func_0203daac());
}
