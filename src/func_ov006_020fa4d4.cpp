//cpp
extern "C" {
extern void func_ov006_020f9760(void* p);
extern void func_ov006_020c1604(char* c, int unused, short a2, void* a3);
extern void func_ov004_020adb1c(int self);
extern int data_ov006_0213d6fc;
void func_ov006_020fa4d4(char* c) {
  func_ov006_020f9760(c + 0x51a8);
  data_ov006_0213d6fc = 0;
  *(short*)(c + 0x592e) = 0;
  *(short*)(c + 0x511e) = 1;
  func_ov006_020c1604(c + 0x4f38, 4, 4, c + 0x592e);
  *(short*)(c + 0x4f52) = 1;
  *(short*)(c + 0x592a) = 0;
  func_ov004_020adb1c(0);
  *(short*)(c + 0x5928) = 1;
}
}
