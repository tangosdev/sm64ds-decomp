//cpp
extern "C" {
extern void func_ov006_021050bc(void* c);
void func_ov006_0210713c(char* c) {
  if (*(int*)(c + 0x4cac) == 0) {
    func_ov006_021050bc(c);
    *(unsigned char*)(c + 0x4fe7) = 0;
    *(unsigned char*)(c + 0x4fe8) = 2;
    return;
  }
  *(unsigned char*)(((int)c + 0x4fe7)) += 1;
  if (*(unsigned char*)(c + 0x4fe7) >= 8) {
    *(unsigned char*)(c + 0x4fe7) = 0;
    *(unsigned char*)(((int)c + 0x4fe8)) -= 1;
  }
  if (*(unsigned char*)(c + 0x4fe8) == 0)
    *(int*)(c + 0x4ca8) = 2;
}
}
