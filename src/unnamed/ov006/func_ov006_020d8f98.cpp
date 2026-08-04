//cpp
extern "C" {
extern int data_ov006_021416a0[];
extern void func_ov006_020d65c8(void*);
extern void func_ov006_020d6278(void*);
extern void func_ov006_020d5fec(void*);
void func_ov006_020d8f98(unsigned char* c){
  int idx=*(int*)(c+0x62d4);
  int* e=&data_ov006_021416a0[idx*2];
  int off=e[1];
  void* obj=c+(off>>1);
  void(*f)(void*);
  if(off&1) f=(void(*)(void*))(*(int*)(*(int*)obj+e[0]));
  else f=(void(*)(void*))e[0];
  f(obj);
  func_ov006_020d65c8(c);
  func_ov006_020d6278(c);
  func_ov006_020d5fec(c);
}
}
