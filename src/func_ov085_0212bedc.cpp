//cpp
typedef int Fix12;
struct Vector3 { int x; int y; int z; };
struct Mtx43 { int m[12]; };

extern "C" int Vec3_HorzDist(const Vector3 *a, const Vector3 *b);
extern "C" short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern "C" void Matrix4x3_FromRotationY(void *m, int angle);
extern "C" void MulVec3Mat4x3(const Vector3 *v, const Mtx43 *m, Vector3 *res);
extern "C" void Matrix4x3_FromTranslation(Mtx43 *m, int x, int y, int z);

struct ShadowModel;
struct Matrix4x3;

struct Actor {
    void DropShadowRadHeight(ShadowModel &, Matrix4x3 &, Fix12, Fix12, unsigned int);
};

extern Mtx43 data_020a0e68;

struct Obj {
    char pad5c[0x5c];
    int v5c;
    int v60;
    int v64;
    char pad3c0[0x3c0 - 0x68];
    char shadowmodel[0x28];
    Mtx43 mtx;
};

extern "C" void func_ov085_0212bedc(Obj *c)
{
    Vector3 v;
    Vector3 res;
    Vector3 vd;

    v.x = 0; v.y = 0; v.z = 0;
    res.x = 0; res.y = 0; res.z = 0;
    vd.x = 0; vd.y = 0; vd.z = 0;

    vd.x = c->v5c;
    vd.y = c->v60;
    vd.z = c->v64;
    vd.x = 0x1086000;
    v.z = Vec3_HorzDist((Vector3 *)&c->v5c, &vd);
    Matrix4x3_FromRotationY(&data_020a0e68, Vec3_HorzAngle((Vector3 *)&c->v5c, &vd));
    MulVec3Mat4x3(&v, &data_020a0e68, &res);
    {
        int t;
        vd.x = vd.x + res.x;
        t = c->v60;
        vd.y = t;
        vd.z = vd.z + res.z;
        Matrix4x3_FromTranslation(&data_020a0e68, vd.x >> 3, (t - 0xc000) >> 3, vd.z >> 3);
    }
    c->mtx = data_020a0e68;
    ((Actor *)c)->DropShadowRadHeight(*(ShadowModel *)&c->shadowmodel, *(Matrix4x3 *)&c->mtx, 0x46000, 0x258000, 0xf);
}
