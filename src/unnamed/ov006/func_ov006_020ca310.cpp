//cpp
extern "C" int _Z15ApproachLinear2Rsss(short&, short, short);
extern "C" void func_ov006_020c9c8c(char*, int);
extern "C" void func_ov006_020ca310(char* c){
  if (_Z15ApproachLinear2Rsss(*(short*)(c+0x6c), 0, 1) == 0) {
    *(int*)(c+0x28) = 0;
    return;
  }
  *(int*)(c+0x28) = 0;
  *(int*)(c+0x30) = *(int*)(c+0x24);
  *(int*)(c+0x34) = *(int*)(c+0x28);
  *(int*)(c+0x38) = *(int*)(c+0x2c);
  *(short*)(c+0x6e) = 1;
  func_ov006_020c9c8c(c, 1);
}
