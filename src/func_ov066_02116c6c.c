typedef int Fix12;
struct Vector3 { int x, y, z; };
extern int _ZN9Animation8FinishedEv(void* self);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int a, int x, int y, int z);
extern void _ZN5Actor16TriplePoofDustAtERK7Vector3(void* self, const struct Vector3* v);
extern void func_02012694(int a, void* p);
extern void _ZN9ActorBase18MarkForDestructionEv(void* self);

int func_ov066_02116c6c(char* c) {
  if ((unsigned int)((unsigned int)(*(unsigned int*)(c+0x3b8) << 4) >> 0x10) > 0xc) {
    *(int*)(c+0x98) = 0;
  }
  if (_ZN9Animation8FinishedEv(c+0x3b0) != 0) {
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x7c, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x7d, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
    {
      struct Vector3 v;
      v.x = *(int*)(c+0x5c);
      v.y = *(int*)(c+0x60);
      v.z = *(int*)(c+0x64);
      _ZN5Actor16TriplePoofDustAtERK7Vector3(c, &v);
    }
    func_02012694(0x146, c+0x74);
    _ZN9ActorBase18MarkForDestructionEv(c);
  }
  return 1;
}
