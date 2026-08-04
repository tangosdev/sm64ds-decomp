extern int data_ov006_02140594;
extern unsigned char* data_ov006_02140550;
extern unsigned char* data_ov006_0214097c[];
extern int func_ov006_020cce0c(unsigned char*);
int func_ov006_020cd658(unsigned char* a0,int a1){
  int i=0;
  int off;
  unsigned char* x;
  data_ov006_02140594=a1;
  data_ov006_02140550=a0;
  if(a1>0){
    off=0;
    do{
      if(func_ov006_020cce0c(data_ov006_02140550+off)==0) return 0;
      x=data_ov006_02140550+off;
      if(i<5) data_ov006_0214097c[i]=x;
      i++;
      off+=0xd0;
    }while(i<data_ov006_02140594);
  }
  return 1;
}
