typedef unsigned short u16;
extern unsigned short DecIfAbove0_Short(unsigned short* p);
extern char* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, char* t);

int func_ov073_021222ec(char* c) {
  if (DecIfAbove0_Short((unsigned short*)(c+0x330)) == 1) {
    char* a = _ZN5Actor15FindWithActorIDEjPS_(0xda, 0);
    if (a != 0) {
      switch (*(u16*)(c+0xc)) {
      case 0xaa:
        *(int*)(c+0x334) = *(int*)(a+4);
        break;
      case 0xab: {
        char* p = a + (*(unsigned char*)(c+0x32d)) * 0xc;
        *(int*)(p+0x3e8) = *(int*)(c+0x5c);
        *(int*)(p+0x3ec) = *(int*)(c+0x60);
        *(int*)(p+0x3f0) = *(int*)(c+0x64);
        break;
      }
      case 0xac: {
        char* p = a + (*(unsigned char*)(c+0x32d)) * 0xc;
        *(int*)(p+0x448) = *(int*)(c+0x5c);
        *(int*)(p+0x44c) = *(int*)(c+0x60);
        *(int*)(p+0x450) = *(int*)(c+0x64);
        break;
      }
      }
    }
  }
  return 1;
}
