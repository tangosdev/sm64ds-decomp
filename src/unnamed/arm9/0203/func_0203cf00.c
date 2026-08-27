typedef int s32;

struct Vector3 {
    int x;
    int y;
    int z;
};

extern void CrossVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *result);

void func_0203cf00(struct Vector3 *out, struct Vector3 *a, struct Vector3 *b)
{
    struct Vector3 tmp;
    CrossVec3(a, b, &tmp);
    out->x = tmp.x;
    out->y = tmp.y;
    out->z = tmp.z;
}
