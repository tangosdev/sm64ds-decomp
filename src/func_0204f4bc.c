extern void func_020524c4(void* p);
extern void func_02052494(void* o, int a, int b);

void func_0204f4bc(char* obj){
  *(unsigned char*)(obj + 0x2e) = 0;
  *(unsigned char*)(obj + 0x2d) = 0;
  *(unsigned short*)(obj + 0x34) = 0;
  *(unsigned short*)(obj + 0x3e) = 0;
  *(unsigned char*)(obj + 0x40) = 0x7f;
  *(unsigned char*)(obj + 0x41) = 0x7f;
  func_020524c4(obj + 0x1c);
  func_02052494(obj + 0x1c, 0x7f00, 1);
}
