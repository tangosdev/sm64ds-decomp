//cpp
typedef short s16;
struct Vector3;
extern "C" s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern "C" s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
void UpdateAngle(short &, short, int, short);

struct Model {
    void Render(const Vector3 *);
};

struct C {
    char pad10[0x10];
    Model model;
    char pad[0x80 - 0x14];
    int v80;
    char pad8c[0x8c - 0x84];
    int v8c;
    char pad9a[0x9a - 0x90];
    s16 v9a;
    char pada2[0xa2 - 0x9c];
    s16 va2;
    s16 va4;
    char pada8[0xa8 - 0xa6];
    int va8;
};

extern "C" void func_ov006_020c1f4c(C *c);

extern "C" void func_ov006_020c201c(C *c)
{
    if (c->va8 != 0) {
        s16 h = Vec3_HorzAngle((const Vector3 *)&c->v8c, (const Vector3 *)&c->v80);
        s16 v = Vec3_VertAngle((const Vector3 *)&c->v8c, (const Vector3 *)&c->v80);
        s16 dh = (s16)(h - c->v9a);
        if (dh < -0x2800) dh = -0x2800;
        else if (dh > 0x2800) dh = 0x2800;
        if (v < -0x1000) v = -0x1000;
        else if (v > 0x1000) v = 0x1000;
        UpdateAngle(c->va4, dh, 8, 0x200);
        UpdateAngle(c->va2, v, 8, 0x200);
        dh = (s16)(h - dh);
        UpdateAngle(c->v9a, dh, 8, 0x200);
    } else {
        UpdateAngle(c->va2, 0, 8, 0x200);
        UpdateAngle(c->va4, 0x800, 8, 0x200);
        UpdateAngle(c->v9a, -0x1400, 8, 0x200);
    }
    func_ov006_020c1f4c(c);
    c->model.Render(0);
}
