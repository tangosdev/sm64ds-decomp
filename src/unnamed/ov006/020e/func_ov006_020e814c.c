extern void _ZN9ModelAnimC1Ev(void* p);
extern void _ZN15TextureSequenceC1Ev(void* p);
extern void* func_ov006_020e7aac(char* p);

char* func_ov006_020e814c(char* c) {
  *(int*)(c) = 0x8c000;
  *(int*)(c + 4) = 0x8c000;
  *(int*)(c + 8) = 0;
  _ZN9ModelAnimC1Ev(c + 0xc);
  _ZN15TextureSequenceC1Ev(c + 0x70);
  func_ov006_020e7aac(c + 0x84);
  *(int*)(c + 0x20c) = 0;
  *(short*)(c + 0x218) = 0;
  *(unsigned char*)(c + 0x21a) = 0x1f;
  return c;
}
