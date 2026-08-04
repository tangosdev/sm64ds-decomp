//cpp
struct C; typedef void (C::*PMF)(int);
extern PMF data_ov006_02142840[];
extern "C" {
extern void func_ov006_021050bc(void* c);
extern void func_ov006_02105854(void* c);
}
struct C { char pad[0x8000]; };
extern "C" void func_ov006_02106f44(char* c) {
  int i;
  func_ov006_021050bc(c);
  *(unsigned char*)(c + 0x4fe9) = 1;
  func_ov006_02105854(c);
  for (i = 0; i < *(int*)(c + 0x4cb8); i++) {
    unsigned char idx = *(unsigned char*)(c + i + 0x4efa);
    (((C*)c)->*data_ov006_02142840[idx])(i);
  }
}
