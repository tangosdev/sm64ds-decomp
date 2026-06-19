//cpp
struct V3 { int x, y, z; };
struct MCB {
  virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
  virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
  virtual void v8(); virtual void v9();
  virtual int v10(V3*, void*);
};
extern "C" void _ZN16MeshColliderBase22UpdatePosWithTransformERS_P5ActorR10ClsnResultR7Vector3P10Vector3_16S8_(
    MCB *self, void *a1, void *a2, V3 *v) {
  V3 local; local.x=v->x; local.y=v->y; local.z=v->z;
  if (self->v10(&local,v) == 0) return;
  return;
}
