extern void func_ov006_020c19d0(void*);
extern void* func_ov006_020dac34(void*);
extern void func_ov004_020b65e4(void*);

int func_ov006_020dabec(char* c){
  *(short*)(((int)c+0x5396) & 0xFFFFFFFFFFFFFFFF) += 1;
  func_ov006_020c19d0(c+0x4f38);
  func_ov004_020b65e4(func_ov006_020dac34(c));
  return 1;
}
