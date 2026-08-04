struct S{int w[2];};
extern struct S data_ov006_0213af80;
extern void func_ov006_020e6e3c(int a, int b);
void func_ov006_020c4f68(int *c){
  *(short*)((char*)c+0xea)=0;
  func_ov006_020e6e3c(0x1c8, *(int*)((char*)c+0x9c));
  *(struct S*)((char*)c+0x30)=data_ov006_0213af80;
}
