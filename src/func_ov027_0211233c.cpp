//cpp
extern "C" {
extern void* _ZN8dActor_c15FindWithActorIDEjPS_(unsigned int, void*);
extern int Vec3_Dist(void*, void*);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
}
extern "C" int func_ov027_0211233c(char* c){
  void* a = _ZN8dActor_c15FindWithActorIDEjPS_(0x102, 0);
  int thr = *(int*)(c+4) + 0x12b000;
  int d = Vec3_Dist(c+0x40, (char*)a+0x5c);
  if (d >= thr)
    return d;
  _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x100, *(int*)(c+0x40), *(int*)(c+0x44), *(int*)(c+0x48));
  *(char*)(c+0x5e) = 0;
  return 0;
}
