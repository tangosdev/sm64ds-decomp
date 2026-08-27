extern void func_0205ad6c(int a, int b);

void func_0204f558(unsigned char* self, int val){
  if (self == 0) return;
  if (val == *(unsigned char*)(self + 0x2e)) return;
  func_0205ad6c(*(unsigned char*)(self + 0x3c), val);
  *(unsigned char*)(self + 0x2e) = (unsigned char)val;
}
