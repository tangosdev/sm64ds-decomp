//cpp
extern "C" {
extern int _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(unsigned int a, unsigned int b, int c, int d, int e, void* f);
int _ZN14FlameChompFire6RenderEv(char* c) {
  int b = (*(int*)(c + 0xb0) & 0x40000) != 0;
  if (b) return 1;
  *(int*)(c + 0x324) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
      *(int*)(c + 0x324), 0x7f, *(int*)(c + 0x5c), *(int*)(c + 0x60) + 0x4b000, *(int*)(c + 0x64), 0);
  *(int*)(c + 0x328) = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
      *(int*)(c + 0x328), 0x80, *(int*)(c + 0x5c), *(int*)(c + 0x60) + 0x4b000, *(int*)(c + 0x64), 0);
  return 1;
}
}
