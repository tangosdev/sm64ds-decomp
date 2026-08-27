typedef long long s64;
typedef struct { int x, y, z; } Vec3;
typedef struct { int x, y, z, w; } Quat;

extern int NormalizeVec3IfNonZero(Vec3 *v);
extern int DotVec3(const Vec3 *a, const Vec3 *b);
extern int func_020531a4(int a);
extern void func_0203cebc(Vec3 *out, Vec3 *p, Vec3 *q, Vec3 *r);
extern int _ZN4cstd4fdivEii(int a, int b);
extern Quat data_02075b04;

void Quaternion_FromVector3(Quat *out, Vec3 *a, Vec3 *b)
{
    Vec3 vb, va, vb2, vcross;
    int r4, absr4, q;

    vb.x = b->x;
    vb.y = b->y;
    vb.z = b->z;
    va.x = a->x;
    va.y = a->y;
    va.z = a->z;
    if (NormalizeVec3IfNonZero(&vb) != 0 && NormalizeVec3IfNonZero(&va) != 0) {
        r4 = func_020531a4((DotVec3(&va, &vb) + 0x1000) << 1);
        vb2.x = vb.x;
        vb2.y = vb.y;
        vb2.z = vb.z;
        func_0203cebc(&vcross, &vb, &va, &vb2);
        vb.x = vcross.x;
        vb.y = vcross.y;
        vb.z = vcross.z;
        absr4 = (r4 < 0) ? -r4 : r4;
        if (absr4 > 4) {
            q = _ZN4cstd4fdivEii(0x1000, r4);
            out->x = (int)(((s64)vb.x * q + 0x800) >> 12);
            out->y = (int)(((s64)vb.y * q + 0x800) >> 12);
            out->z = (int)(((s64)vb.z * q + 0x800) >> 12);
            out->w = r4 >> 1;
            return;
        }
        out->x = data_02075b04.x;
        out->y = data_02075b04.y;
        out->z = data_02075b04.z;
        out->w = data_02075b04.w;
        return;
    }
    out->x = data_02075b04.x;
    out->y = data_02075b04.y;
    out->z = data_02075b04.z;
    out->w = data_02075b04.w;
}
