extern int data_0209b308[];
extern unsigned func_ov004_020adc3c(void* c);
extern unsigned func_02013580(unsigned a, int b);
int func_ov004_020ad6fc(void* c, int v){
  int i;
  if(data_0209b308[2]==5){
    if(v<=0) return -1;
    for(i=0;i<5;i++){
      if(func_02013580(func_ov004_020adc3c(c), i) > (unsigned)v) return i;
    }
  } else {
    for(i=0;i<5;i++){
      if(func_02013580(func_ov004_020adc3c(c), i) < (unsigned)v) return i;
    }
  }
  return -1;
}
