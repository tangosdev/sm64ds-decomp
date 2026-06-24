//cpp
extern "C" {
extern void func_ov006_02100734(char* c, int idx);
extern unsigned func_02054de8(void);
extern void MultiStore16(unsigned short val, char* dst, int nbytes);
extern void func_ov006_02102dbc(void* c);
void func_ov006_0210258c(char* c) {
  volatile unsigned short v;
  char* dst;
  if (*(unsigned short*)(c + 0x5674) != 0) return;
  func_ov006_02100734(c, 0);
  func_ov006_02100734(c, 1);
  func_ov006_02100734(c, 2);
  *(int*)(c + 0x5660) = 2;
  *(short*)(c + 0x566e) = 0x40;
  dst = (char*)func_02054de8();
  v = 0;
  MultiStore16(v, dst, 0x6000);
  *(unsigned char*)(c + 0x5677) = 0;
  func_ov006_02102dbc(c);
}
}
