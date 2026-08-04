extern void data_ov007_020cd72c(int *c);
extern int data_ov006_0213b3e0[];
int *func_ov006_020cd6f4(int *c){
  data_ov007_020cd72c(c);
  *c=(int)data_ov006_0213b3e0;
  *(short*)((char*)c+0x20)=0;
  return c;
}
