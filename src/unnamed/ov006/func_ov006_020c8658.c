struct S{int w[2];};
extern struct S data_ov006_0213b048;
void func_ov006_020c8658(void *c){
  *(char*)((char*)c+0x35)=0;
  *(int*)((char*)c+0x18)=0;
  *(struct S*)((char*)c+0x3c)=data_ov006_0213b048;
}
