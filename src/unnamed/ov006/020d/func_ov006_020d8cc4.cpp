//cpp
extern "C" int func_ov006_020d836c(void);
extern "C" void func_ov006_020d7604(void*);
extern "C" void func_ov006_020d5e1c(void*);
#pragma opt_strength_reduction off
extern "C" void func_ov006_020d8cc4(char* r5){
  func_ov006_020d836c();
  int c4 = 0;
  int c2 = 0;
  int i = 0;
  do {
    char* e = r5 + (i << 6);
    e = e + 0x4000;
    if (*(unsigned char*)(e+0x698) != 0 && *(unsigned char*)(e+0x697) == 6) {
      unsigned char v = *(unsigned char*)(e+0x69b);
      if (v != 2) c2++;
      if (v != 4) c4++;
    }
    i++;
  } while (i < 0x70);
  if (c2 == 0 && *(unsigned char*)(r5+0x6000+0x2f4) == 0) {
    func_ov006_020d7604(r5);
  }
  if (c4 != 0) return;
  func_ov006_020d5e1c(r5);
  *(short*)(r5+0x6200+0xe8) = 0x10;
  *(int*)(r5+0x6000+0x2d4) = 2;
  *(unsigned char*)(r5+0x6000+0x2f9) = 1;
}
