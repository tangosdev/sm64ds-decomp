struct Vector3;
struct Vec3 { int x, y, z; };

extern int func_020092c4(void *self, struct Vec3 *out, struct Vec3 *target);
extern short Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern short Vec3_VertAngle(const struct Vector3 *v1, const struct Vector3 *v0);

void func_0200cbe0(void *self)
{
    int r1 = func_020092c4(self, (struct Vec3 *)((char *)self + 0x80), (struct Vec3 *)((char *)self + 0xb0));
    int r2 = func_020092c4(self, (struct Vec3 *)((char *)self + 0x8c), (struct Vec3 *)((char *)self + 0xbc));

    if (r2 != 0) {
        if (r1 != 0) {
            *(int *)(((long long)(int)((char *)self + 0x154)) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x8000;
        }
    }

    short h = Vec3_HorzAngle((const struct Vector3 *)((char *)self + 0x80),
                             (const struct Vector3 *)((char *)self + 0x8c));
    *((short *)((char *)self + 0x100 + 0x7c)) = h;

    short v = Vec3_VertAngle((const struct Vector3 *)((char *)self + 0x80),
                             (const struct Vector3 *)((char *)self + 0x8c));
    *((short *)((char *)self + 0x100 + 0x7e)) = v;
}
