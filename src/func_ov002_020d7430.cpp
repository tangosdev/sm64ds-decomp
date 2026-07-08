//cpp
struct Vec3 { int x, y, z; };
struct Obj {
  virtual int v0(); virtual int v1(); virtual int v2(); virtual int v3();
  virtual int v4(); virtual int v5(); virtual int v6(); virtual int v7();
  virtual int v8(); virtual int v9(); virtual int v10(); virtual int v11();
  virtual int v12(); virtual int v13(); virtual int v14(); virtual int v15();
  virtual int v16(); virtual int v17(); virtual int v18(); virtual int v19(char* c);
};
extern "C" {
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(char* c, Vec3* v, unsigned int a, int fix, unsigned int b, unsigned int d, unsigned int e);
extern void func_ov002_020c9e18(char* c);
extern int _ZN6Player7IsStateERNS_5StateE(char* c, void* s);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* s);
extern void func_ov002_020d718c(char* c);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(unsigned int a, unsigned int b, void* v);
extern char _ZN6Player14ST_YOSHI_POWERE;
extern char _ZN6Player7ST_WALKE;
void func_ov002_020d7430(char* c){
  Obj* o = *(Obj**)(c+0x360);
  if(o->v18() == 2){
    Vec3 v;
    v.x = *(int*)(c+0x5c);
    v.y = *(int*)(c+0x60);
    v.z = *(int*)(c+0x64);
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(c, &v, 1, 0xc000, 1, 0, 0);
    return;
  }
  func_ov002_020c9e18(c);
  o = *(Obj**)(c+0x360);
  o->v19(c);
  if(_ZN6Player7IsStateERNS_5StateE(c, &_ZN6Player14ST_YOSHI_POWERE)){
    _ZN6Player11ChangeStateERNS_5StateE(c, &_ZN6Player7ST_WALKE);
  }
  func_ov002_020d718c(c);
  _ZN5Sound13PlayCharVoiceEjjRK7Vector3(0, 0xfd, c+0x74);
}
}
