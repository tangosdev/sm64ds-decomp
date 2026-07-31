//cpp
extern "C" {
struct Vector3 { int x, y, z; };

extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern void _ZN12WithMeshClsn19ClearAllGroundFlagsEv(void *self);
extern void _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(void *self, struct Vector3 *v, int fix, void *actor);
extern int func_02038a38(void *arg0);
extern void func_020371b0(void *clsn, int justHit);
extern int _ZNK12WithMeshClsn15ShouldUpdatePosEv(void *self);
extern int _ZNK12WithMeshClsn16ShouldUpdatePosYEv(void *self);
extern void func_020356d4(char *self);
}

struct WithMeshClsn {
    void UpdateDiscreteNoLava_2();
};

void WithMeshClsn::UpdateDiscreteNoLava_2()
{
    char * self = (char *)this;
int onGround;
    int sy;
    struct Vector3 v;
    struct Vector3 *p6c;
    struct Vector3 *src = (struct Vector3 *)(*(char **)(self + 0x14) + 0x5c);
    char *obj = *(char **)(self + 0x14);

    onGround = _ZNK12WithMeshClsn10IsOnGroundEv(self);
    _ZN12WithMeshClsn19ClearAllGroundFlagsEv(self);
    v.x = *(int *)(((long long)(int)src) & 0xffffffffffffffffLL);
    sy = src->y;
    v.y = sy;
    v.z = src->z;
    v.y = sy + *(int *)(self + 0x1c);
    _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(self + 0x20, &v,
        *(int *)(self + 0x18), *(void **)(self + 0x14));
    *(int *)(self + 0x128) = *(int *)(self + 0x1b8);
    if (src->y - *(int *)(obj + 0x6c) > 0) {
        *(unsigned char *)(((long long)(int)(self + 0x90)) & 0xffffffffffffffffLL) |= 0x20;
    }
    if (func_02038a38(self + 0x20)) {
        p6c = (struct Vector3 *)(self + 0x6c);
        if (*(unsigned char *)(self + 0x90) & 4) {
            func_020371b0(self, onGround);
        }
        if (_ZNK12WithMeshClsn15ShouldUpdatePosEv(self)) {
            src->x += p6c->x;
            if (_ZNK12WithMeshClsn16ShouldUpdatePosYEv(self)) {
                *(int *)(((long long)(int)((char *)src + 4)) & 0xffffffffffffffffLL) += p6c->y;
            }
            *(int *)(((long long)(int)((char *)src + 8)) & 0xffffffffffffffffLL) += p6c->z;
        }
    }
    if (onGround == 0)
        return;
    if (_ZNK12WithMeshClsn10IsOnGroundEv(self))
        return;
    func_020356d4(self);

}
