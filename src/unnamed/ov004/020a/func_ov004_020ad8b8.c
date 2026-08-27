extern int data_ov004_020beb68;
extern int data_0209b308[];
extern int func_ov004_020adc3c(void);
extern int func_02013580(int a, int b);
int func_ov004_020ad8b8(void){
  if(data_ov004_020beb68 != 0){
    int r=func_02013580(func_ov004_020adc3c(),0);
    if(r==0) return data_0209b308[0x14/4];
    return r;
  }
  return 0;
}
