//cpp
extern "C" {
struct Vector3 { int x, y, z; };
struct Actor;
struct RaycastLine { char data[0x78]; };

extern void func_020374b8(int *a, int *b);
extern void func_0203aa74(void *thiz, Vector3 *v, Vector3 *res);
extern void _ZN11RaycastLineC1Ev(RaycastLine *);
extern void _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(RaycastLine *, const Vector3 *, const Vector3 *, Actor *);
extern void func_02035394(RaycastLine *, void *);
extern int _ZN12MeshCollider10DetectClsnER11RaycastLine(void *, RaycastLine *);
extern Vector3 *_ZN11RaycastLine10GetClsnPosEv(Vector3 *, RaycastLine *);
extern void func_0203aa10(void *thiz, const Vector3 *v, Vector3 *res);
extern void _ZN10ClsnResultaSERKS_(void *, const void *);
extern void _ZN11RaycastLineD1Ev(RaycastLine *);

int _ZN21ExtendingMeshCollider10DetectClsnER13RaycastGround(void *self, char *ground)
{
    Vector3 v0, v0xc, v0x18, v0x24, v0x30, v0x3c;
    RaycastLine ray;

    func_020374b8((int *)ground, (int *)&v0xc);
    v0x24 = v0xc;

    int b4c = *(int *)(ground + 0x4c);
    if (*(unsigned char *)(ground + 0x48) != 0) {
        int diff = v0xc.y - *(int *)(ground + 0x44);
        if (diff < b4c) b4c = diff;
    }
    v0x24.y = v0x24.y - b4c;

    func_0203aa74(self, &v0xc, &v0);
    func_0203aa74(self, &v0x24, &v0x18);

    _ZN11RaycastLineC1Ev(&ray);
    _ZN11RaycastLine13SetObjAndLineERK7Vector3S2_P5Actor(&ray, &v0, &v0x18, 0);
    func_02035394(&ray, ground);
    int r = _ZN12MeshCollider10DetectClsnER11RaycastLine(self, &ray);
    if (r) {
        _ZN11RaycastLine10GetClsnPosEv(&v0x30, &ray);
        func_0203aa10(self, &v0x30, &v0x3c);
        _ZN10ClsnResultaSERKS_(ground + 0x10, ray.data + 0x10);
        *(int *)(ground + 0x44) = v0x3c.y;
        *(unsigned char *)(ground + 0x48) = 1;
    }
    _ZN11RaycastLineD1Ev(&ray);
    return r;
}
}
