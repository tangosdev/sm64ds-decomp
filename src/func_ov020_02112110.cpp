//cpp
extern "C" {
int _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int a, unsigned int b, void* pos, void* rot, int e, int f);
void _ZN5Actor8PoofDustEv(void* self);
void _ZN9ActorBase18MarkForDestructionEv(void* self);
void func_ov020_02112110(char* c) {
  if (*(unsigned char*)(c+0x108)) {
    int param = *(signed char*)(c+0xcc);
    _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0x122, 2, (void*)(c+0x5c), (void*)0, param, -1);
  }
  _ZN5Actor8PoofDustEv(c);
  _ZN9ActorBase18MarkForDestructionEv(c);
}
}
