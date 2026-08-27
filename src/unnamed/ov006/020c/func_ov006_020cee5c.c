extern int data_ov006_02140838;
extern unsigned char* data_ov006_0214082c;
extern int data_ov006_02140828;
extern int data_ov006_02140818;
extern int data_ov006_02140830;
extern void func_ov006_020cecb4(int* p);
void func_ov006_020cee5c(void){
  int i=0;
  int off;
  if(data_ov006_02140838>0){
    off=0;
    do{
      func_ov006_020cecb4((int*)(data_ov006_0214082c+off));
      i++;
      off+=0x1d0;
    }while(i<data_ov006_02140838);
  }
  data_ov006_02140828=0;
  data_ov006_02140818=0;
  data_ov006_02140830=0;
}
