struct Vector3;
struct Camera;

extern short Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern short Vec3_VertAngle(const struct Vector3 *v1, const struct Vector3 *v0);

void func_0200928c(struct Camera *cam)
{
    short h = Vec3_HorzAngle((const struct Vector3 *)((char *)cam + 0x80),
                             (const struct Vector3 *)((char *)cam + 0x8c));
    *((short *)((char *)cam + 0x100 + 0x7c)) = h;

    short v = Vec3_VertAngle((const struct Vector3 *)((char *)cam + 0x80),
                             (const struct Vector3 *)((char *)cam + 0x8c));
    *((short *)((char *)cam + 0x100 + 0x7e)) = v;
}
