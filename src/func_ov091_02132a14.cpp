//cpp
// func_ov091_02132a14 at 0x02132a14
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov091).
struct Vector3 { int x, y, z; };
struct VObj {
  virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
  virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
  virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
  virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
  virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
  virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
  virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
  virtual void v28(); virtual int  vGet();
};
extern "C" {
void _ZN6Player16IncMegaKillCountEv(void* self);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
void _ZN5Actor10PoofDustAtERK7Vector3(void* self, const Vector3& v);
void _ZN9ActorBase18MarkForDestructionEv(void* self);
void func_02012694(int, void*);
}

extern "C" void func_ov091_02132a14(char* c, void* p){
    _ZN6Player16IncMegaKillCountEv(p);
    Vector3 vec;
    vec.x = *(int*)(c + 0x5c);
    vec.y = *(int*)(c + 0x60);
    vec.z = *(int*)(c + 0x64);
    int ret = ((VObj*)c)->vGet();
    vec.y += ret;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x48, vec.x, vec.y, vec.z);
    Vector3 vec2;
    ((int*)&vec2)[0] = ((int*)&vec)[0];
    ((int*)&vec2)[1] = ((int*)&vec)[1];
    ((int*)&vec2)[2] = ((int*)&vec)[2];
    _ZN5Actor10PoofDustAtERK7Vector3(c, vec2);
    _ZN9ActorBase18MarkForDestructionEv(c);
    func_02012694(0x1e, c + 0x74);
}