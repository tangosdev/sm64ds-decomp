//cpp
extern "C" {
struct Vector3 { int x,y,z; };
struct dActor_c;
struct dBgCh_Lin { char data[0x78]; };
extern void _ZN9dBgCh_LinC1Ev(dBgCh_Lin*);
extern void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(dBgCh_Lin*, const Vector3*, const Vector3*, dActor_c*);
extern int _ZN9dBgCh_Lin10DetectClsnEv(dBgCh_Lin*);
extern Vector3* func_02037dc4(void*);
extern void _ZN9dBgCh_LinD1Ev(dBgCh_Lin*);

int _ZN8dActor_c17DetectRaycastClsnER7Vector3S1_b(dActor_c *self, Vector3 *a, Vector3 *out, int doStore){
  dBgCh_Lin rl;
  _ZN9dBgCh_LinC1Ev(&rl);
  _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(&rl, a, (const Vector3*)out, 0);
  if(_ZN9dBgCh_Lin10DetectClsnEv(&rl)){
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
    _ZN9dBgCh_LinD1Ev(&rl);
    return 1;
  }
  _ZN9dBgCh_LinD1Ev(&rl);
  return 0;
}
}
