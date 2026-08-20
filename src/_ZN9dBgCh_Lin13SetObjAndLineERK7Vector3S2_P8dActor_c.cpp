//cpp
struct Vector3 { int x, y, z; };
struct dActor_c;

extern "C" {
void _ZN8dM3dGLin3SetERK7Vector3S2_(void *line, const Vector3 *a, const Vector3 *b);
void func_020353b0(char *c, void *p);
void func_02037608(char *thiz);
void func_0203abd4(int *dst, Vector3 *mid, int w);
}

extern "C" void _ZN9dBgCh_Lin13SetObjAndLineERK7Vector3S2_P8dActor_c(
    char *thiz, const Vector3 *a, const Vector3 *b, dActor_c *obj)
{
    Vector3 mid;
    _ZN8dM3dGLin3SetERK7Vector3S2_(thiz + 0x38, a, b);
    func_020353b0(thiz, obj);
    func_02037608(thiz);
    mid.x = a->x + b->x;
    mid.y = a->y + b->y;
    mid.z = a->z + b->z;
    mid.x >>= 1;
    mid.y >>= 1;
    mid.z >>= 1;
    func_0203abd4((int *)(thiz + 0x64), &mid, (*(int *)(thiz + 0x60) >> 1) + 0x1000);
}
