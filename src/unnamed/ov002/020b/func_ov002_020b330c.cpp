//cpp
extern "C" {
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned, void*);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned, int, int, int);
extern void _ZN7fBase_c18MarkForDestructionEv(void*);
void func_ov002_020b330c(char* c){
  _ZN5Sound9PlayBank3EjRK7Vector3(0xb, c+0x74);
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(2, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  _ZN7fBase_c18MarkForDestructionEv(c);
}
}
