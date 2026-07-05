//cpp
extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int a, int b, int c);
extern void func_02012694(int a, void* p);
extern void _ZN9ActorBase18MarkForDestructionEv(void* a);
void func_ov079_02126794(char* c) {
  if (*(unsigned short*)(c + 0x100) == 0) {
    *(int*)(c + 0x98) = -0x1e000;
    int* p128 = (int*)(((int)c + 0x128) & 0xFFFFFFFFFFFFFFFF);
    *p128 |= 1;
  }
  short* p8c = (short*)(((int)c + 0x8c) & 0xFFFFFFFFFFFFFFFF);
  int* p60 = (int*)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF);
  unsigned short* p100 = (unsigned short*)(((int)c + 0x100) & 0xFFFFFFFFFFFFFFFF);
  *p8c += 0x2000;
  *p60 += 0x14000;
  if (*p100 <= 0x5a) return;
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8f, *(int*)(c + 0x5c), *(int*)(c + 0x60), *(int*)(c + 0x64));
  func_02012694(0x78, c + 0x74);
  _ZN9ActorBase18MarkForDestructionEv(c);
}
}
