//cpp
extern "C" {
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned, void*);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned, int, int, int);
extern void _ZN9ActorBase18MarkForDestructionEv(void*);
void func_ov018_02112880(char* c){
  _ZN5Sound9PlayBank3EjRK7Vector3(0x41, c+0x74);
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x74, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x75, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x76, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  _ZN9ActorBase18MarkForDestructionEv(c);
}
}
