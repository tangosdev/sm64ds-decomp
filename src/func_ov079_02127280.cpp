//cpp
// @symbol func_ov079_02127280
// @emits FortressWall_Kill
/* recovered: renamed to Class_Method */
/* daObjBk_Kabe_c::Kill - recovered from vtable slot identity */
extern "C" {
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned, void*);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned, int, int, int);
extern void _ZN9ActorBase18MarkForDestructionEv(void*);
void FortressWall_Kill(char* c){
  _ZN5Sound9PlayBank3EjRK7Vector3(0xf, c+0x74);
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x121, *(int*)(c+0x5c), *(int*)(c+0x60), *(int*)(c+0x64));
  int b = (int)(*(unsigned short*)(c+0xc) == 0x30);
  if(b){
    *(unsigned char*)(c+0x321) = 1;
    return;
  }
  _ZN9ActorBase18MarkForDestructionEv(c);
}
}
