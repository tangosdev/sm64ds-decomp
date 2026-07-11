//cpp
struct BF3ae {
    unsigned char b0 : 1;
    unsigned char : 4;
    unsigned char sel : 3;
};
extern "C" {
extern void func_ov002_020b1008(void* c);
extern int _ZN5Event6GetBitEj(unsigned int b);
extern void* _ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void* start);
void func_ov002_020b20b4(char* c){
  func_ov002_020b1008(c);
  if (((struct BF3ae*)(c+0x3ae))->b0) return;
  if (_ZN5Event6GetBitEj(*(unsigned char*)(c+0x3ab)) == 0) return;
  ((struct BF3ae*)(int)(((long long)(int)((int)c + 0x3ae)) & 0xFFFFFFFFFFFFFFFFLL))->b0 = 1;
  *(int*)(c+0x3a4) = 4;
  *(int*)(int)(((long long)(int)((int)c + 0x190)) & 0xFFFFFFFFFFFFFFFFLL) &= ~1;
  char* found = (char*)_ZN5Actor15FindWithActorIDEjPS_(0xa, 0);
  if (found) {
    *(short*)(c+0x3a8) = *(unsigned short*)(found+0x32a);
  } else {
    *(short*)(c+0x3a8) = 0xfa;
  }
}
}
