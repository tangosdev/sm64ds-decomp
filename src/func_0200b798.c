typedef long long s64;

typedef struct Vector3 { int x, y, z; } Vector3;
typedef struct dBgCh_Gnd { int _[0x15]; } dBgCh_Gnd;
typedef struct Entry3 { int a, b, c; } Entry3;

extern void _ZN9dBgCh_GndC1Ev(dBgCh_Gnd *t);
extern void _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(dBgCh_Gnd *t, const Vector3 *pos, void *actor);
extern int _ZN9dBgCh_Gnd10DetectClsnEv(dBgCh_Gnd *t);
extern void _ZN9dBgCh_GndD1Ev(dBgCh_Gnd *t);
extern int LenVec3(Vector3 *v);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalarInPlace(int *v, int s);
extern void Vec3_Add(Vector3 *out, Vector3 *a, Vector3 *b);
extern int func_020092c4(int self, Vector3 *out, Vector3 *target);
extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);

extern Entry3 data_02086f58[];

void func_0200b798(char *self, short *a1, int arg2)
{
    dBgCh_Gnd ray;
    Vector3 pos;
    Vector3 v1;
    Vector3 v2;
    Vector3 v3;
    Vector3 v4;
    int idx;
    Entry3 *e;
    int len;
    int limit;
    int scale;
    int c;
    int sum;

    idx = *((unsigned char *)a1 + 1);
    if (idx == 0xff) idx = 0;

    e = &data_02086f58[idx];
    {
        int tz = a1[3] << 12;
        int ty = a1[2] << 12;
        int tx = a1[1] << 12;
        pos.x = tx;
        pos.y = ty;
        pos.z = tz;
    }

    _ZN9dBgCh_GndC1Ev(&ray);
    _ZN9dBgCh_Gnd12SetObjAndPosERK7Vector3P8dActor_c(&ray, &pos, 0);
    if (_ZN9dBgCh_Gnd10DetectClsnEv(&ray) != 0) {
        pos.y -= ray._[0x11];
    }

    sum = arg2 + pos.y;
    {
        int sz = *(int *)(self + 0xa0);
        int sx = *(int *)(self + 0x98);
        int dz = sz - pos.z;
        int dx = sx - pos.x;
        int dy = sum - pos.y;
        v1.x = dx;
        v1.y = dy;
        v1.z = dz;
    }
    c = e->c;
    v2.y = sum;
    v2.x = pos.x + (int)(((s64)v1.x * c + 0x800) >> 12);
    v2.z = pos.z + (int)(((s64)v1.z * c + 0x800) >> 12);

    len = LenVec3(&v1);
    if (len < 0x220000) {
        v2.y += 0x220000 - len;
    }

    Vec3_Sub(&v3, &v2, (Vector3 *)(self + 0x80));
    v1.x = v3.x;
    v1.y = v3.y;
    v1.z = v3.z;
    len = LenVec3(&v1);

    limit = e->a;
    if (*(int *)(self + 0x154) & 0xc00) limit = e->b;

    if (len > limit) {
        scale = _ZN4cstd4fdivEii(limit, len);
        Vec3_MulScalarInPlace((int *)&v1, scale);
        Vec3_Add(&v4, (Vector3 *)(self + 0x80), &v1);
        v2.x = v4.x;
        v2.y = v4.y;
        v2.z = v4.z;
    }

    if (*(int *)(self + 0x154) & 4) {
        *(int *)(self + 0x8c) = v2.x;
        *(int *)(self + 0x90) = v2.y;
        *(int *)(self + 0x94) = v2.z;
    } else {
        func_020092c4((int)self, (Vector3 *)(self + 0x8c), &v2);
    }

    *(short *)(self + 0x17c) =
        Vec3_HorzAngle((Vector3 *)(self + 0x80), (Vector3 *)(self + 0x8c));

    _ZN9dBgCh_GndD1Ev(&ray);
}
