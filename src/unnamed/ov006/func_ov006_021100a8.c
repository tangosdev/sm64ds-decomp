extern void func_ov006_0210fb04(char* c);
extern void func_ov006_0210fa6c(char* c);
extern void func_ov006_0210fb58(char* c);

void func_ov006_021100a8(char* c){
  *(int*)(c+0x10) = *(int*)(c+8);
  *(int*)(c+0x14) = *(int*)(c+0xc);
  if (*(int*)(c+0x7c) > 0) {
    int* p = (int*)(((int)c + 0x7c));
    *p = *p - 1;
  }
  if (*(unsigned char*)(c+0x73) == 0 && *(int*)(c+0x7c) <= 0 && *(int*)(c+0x78) > 0) {
    int* p = (int*)(((int)c + 0x78));
    *p = *p - 1;
    func_ov006_0210fb04(c);
  }
  if (*(int*)(c+0x80) > 0) {
    int* p = (int*)(((int)c + 0x80));
    *p = *p - 1;
    if (*(int*)(c+0x80) == 0)
      func_ov006_0210fa6c(c);
  }
  func_ov006_0210fb58(c);
}
