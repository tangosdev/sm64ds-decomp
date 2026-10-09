typedef int Fix12i;
typedef short s16;

struct Vector3 { Fix12i x, y, z; };

extern s16 _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x);

s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1)
{
    Fix12i dx = v1->x - v0->x;
    Fix12i dz = v1->z - v0->z;
    return _ZN4cstd5atan2E5Fix12IiES1_(dx, dz);
}
