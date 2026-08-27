extern int* data_ov004_020beb68;
extern int func_ov004_020adc3c(int* p);
extern int func_02013580(int a, int b);
int func_ov004_020ad878(void){
  int* p=data_ov004_020beb68;
  if(p==0) return 0;
  return func_02013580(func_ov004_020adc3c(p), 1);
}
