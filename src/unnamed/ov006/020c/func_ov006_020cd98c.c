struct S{int w[2];};
extern struct S data_ov006_0213b36c;
void func_ov006_020cd98c(int *c){
  *(short*)((char*)c+0x9a)=0;
  *(struct S*)c=data_ov006_0213b36c;
}
