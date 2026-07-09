//cpp
extern "C" {

struct Vector3 { int x, y, z; };

extern int _ZNK12WithMeshClsn10IsOnGroundEv(void *self);
extern int func_020355a0(void *p);
extern int _ZNK12WithMeshClsn15ShouldUpdatePosEv(void *self);
extern int func_02037938(int p);
extern void func_02038324(void *arg, int b, int c, int d);
extern void _ZN12WithMeshClsn19ClearAllGroundFlagsEv(void *self);
extern void _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(void *self, struct Vector3 *v, int fix, void *actor);
extern int _ZN10SphereClsn10DetectClsnEv(void *self);
extern void func_020371b0(void *clsn, int justHit);
extern int _ZNK12WithMeshClsn16ShouldUpdatePosYEv(void *self);
extern void func_020356d4(char *self);

void _ZN12WithMeshClsn20UpdateDiscreteNoLavaEv(char *self)
{
    int onGround;
    int sy;
    struct Vector3 v;
    struct Vector3 *p6c;
    char *obj = *(char **)(self + 0x14);
    struct Vector3 *src = (struct Vector3 *)(obj + 0x5c);
    struct Vector3 *p68 = (struct Vector3 *)(obj + 0x68);

    if (_ZNK12WithMeshClsn10IsOnGroundEv(self) && func_020355a0(self)
        && _ZNK12WithMeshClsn15ShouldUpdatePosEv(self)) {
        func_02038324((void *)func_02037938((int)(self + 0x20)), (int)src,
                      *(int *)(self + 0x12c), *(int *)(self + 0x130));
    }
    onGround = _ZNK12WithMeshClsn10IsOnGroundEv(self);
    _ZN12WithMeshClsn19ClearAllGroundFlagsEv(self);
    v.x = src->x;
    sy = src->y;
    v.y = sy;
    v.z = src->z;
    v.y = sy + *(int *)(self + 0x1c);
    _ZN10SphereClsn15SetObjAndSphereERK7Vector35Fix12IiEP5Actor(self + 0x20, &v,
        *(int *)(self + 0x18), *(void **)(self + 0x14));
    *(int *)(self + 0x128) = *(int *)(self + 0x1b8);
    if (src->y - p68->y > 0) {
        *(unsigned char *)(((long long)(int)(self + 0x90)) & 0xffffffffffffffffLL) |= 0x20;
    }
    if (_ZN10SphereClsn10DetectClsnEv(self + 0x20)) {
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
}
