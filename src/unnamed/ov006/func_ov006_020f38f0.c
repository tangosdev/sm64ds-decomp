extern void func_ov004_020b1e34(void* c, int a, int b, int d);
void func_ov006_020f38f0(char* c){
  if(*(unsigned char*)(c+0x5000+0x33d) == 0) return;
  func_ov004_020b1e34(c, 0xe0, 0x14, 1);
}
