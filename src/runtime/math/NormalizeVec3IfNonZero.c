typedef struct Vector3 { int x, y, z; } Vector3;

extern void func_0203ce80(Vector3 *dst, Vector3 *src);

int NormalizeVec3IfNonZero(Vector3 *v)
{
    Vector3 tmp;
    if (v->x == 0 && v->y == 0 && v->z == 0)
        return 0;
    func_0203ce80(&tmp, v);
    return 1;
}
