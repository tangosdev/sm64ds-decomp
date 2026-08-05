//cpp
struct Vector3 { int x,y,z; Vector3(int a,int b,int d){x=a;y=b;z=d;} Vector3(){} };
struct RaycastGround {
  char buf[0x54];
  RaycastGround();
  ~RaycastGround();
  void SetObjAndPos(const Vector3 &pos, void *actor);
  int DetectClsn();
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(void *, const Vector3 &pos, void *actor);

extern "C" int func_ov102_02149610(char *c){
  Vector3 pos(*(int*)(c+0x5c), *(int*)(c+0x60)+0x28000, *(int*)(c+0x64));
  RaycastGround rg;
  _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&(rg), pos, 0);
  *(int*)((char*)&rg + 0x4c) = 0x3e8000;
  int r = *(int*)(c+0x60);
  if (rg.DetectClsn()) r = *(int*)((char*)&rg + 0x44);
  return r;
}
