struct Vector3;
struct Camera;

extern short Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
extern short AngleDiff(short a, short b);

void func_020071a8(struct Camera *cam, short ang, int threshold)
{
    short diff = AngleDiff(
        Vec3_HorzAngle((const struct Vector3 *)((char *)cam + 0x98),
                       (const struct Vector3 *)((char *)cam + 0x8c)),
        ang);
    if (diff > threshold)
        *(short *)((char *)cam + 0x19a) = ang;
}
