//cpp
extern "C" {
struct Vector3 { int x,y,z; };
struct Actor;
struct RaycastLine { char data[0x78]; };
extern void _ZN11RaycastLineC1Ev(RaycastLine*);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(RaycastLine*, const Vector3*, const Vector3*, Actor*);
extern int _ZN11RaycastLine10DetectClsnEv(RaycastLine*);
extern Vector3* func_02037dc4(void*);
extern void _ZN11RaycastLineD1Ev(RaycastLine*);

int _ZN5Actor17DetectRaycastClsnER7Vector3S1_b(Actor *self, Vector3 *a, Vector3 *out, int doStore){
  RaycastLine rl;
  _ZN11RaycastLineC1Ev(&rl);
  _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(&rl, a, (const Vector3*)out, 0);
  if(_ZN11RaycastLine10DetectClsnEv(&rl)){
    if(doStore){
      Vector3 *p = (Vector3*)(rl.data + 0x54);
      Vector3 *q = func_02037dc4(rl.data + 0x14);
      int x = p->x + q->x;
      int z = p->z + q->z;
      int y = p->y + q->y;
      out->x = x;
      out->y = y;
      out->z = z;
    }
    _ZN11RaycastLineD1Ev(&rl);
    return 1;
  }
  _ZN11RaycastLineD1Ev(&rl);
  return 0;
}
}
