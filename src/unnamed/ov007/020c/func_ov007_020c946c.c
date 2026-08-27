extern void* func_ov007_020c3df4(int a, void* b);
extern void func_ov007_020c940c(void* p);
void* func_ov007_020c946c(void* r5){
  char* o = (char*)func_ov007_020c3df4(0, (void*)0x44);
  *(void**)o = r5;
  func_ov007_020c940c(o);
  return o;
}
