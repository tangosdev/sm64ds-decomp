//cpp
extern "C" {
typedef void (*fp)(void*,void*,void*,void*,void*,void*);
struct Obj { char pad[0x18]; fp f; };
void _ZN16MeshColliderBase10BeforeClsnER10ClsnResultP5ActorR7Vector3P10Vector3_16S7_(
    struct Obj *self, void *clsn, void *actor, void *vec, void *v16a, void *v16b) {
  self->f(self, actor, clsn, vec, v16a, v16b);
}
}
