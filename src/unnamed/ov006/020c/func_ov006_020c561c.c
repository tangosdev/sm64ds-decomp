struct S{int w[2];};
extern struct S data_ov006_0213af30;
void func_ov006_020c561c(void *c){
  *(short*)((char*)c+0xe6)=0;
  *(int*)((char*)c+0xa8)=*(int*)((char*)c+4);
  *(int*)((char*)c+0xac)=*(int*)((char*)c+8);
  *(int*)((char*)c+0xb0)=*(int*)((char*)c+0xc);
  *(struct S*)((char*)c+0x30)=data_ov006_0213af30;
}
