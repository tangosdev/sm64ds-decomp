extern void func_0207328c(void* a, int b, int c, void* d);
extern void func_ov004_020b29c0(void* c);
extern void func_ov006_020d116c(void);
extern void func_0203d47c(void);
extern int data_ov006_0213b918[];

void* func_ov006_020d1018(char* c){
  *(void**)c = data_ov006_0213b918;
  func_0207328c(c + 0x4768, 0x80, 0x18, (void*)func_ov006_020d116c);
  func_0207328c(c + 0x4744, 4, 8, (void*)func_0203d47c);
  func_0207328c(c + 0x4724, 4, 8, (void*)func_0203d47c);
  func_0207328c(c + 0x4660, 4, 8, (void*)func_0203d47c);
  func_ov004_020b29c0(c);
  return c;
}
