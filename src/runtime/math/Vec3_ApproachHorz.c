struct Vec3 { int x, y, z; };
extern void Vec3_Sub(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern int Vec3_HorzLen(struct Vec3* v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void Vec3_MulScalar(struct Vec3* out, struct Vec3* in, int s);
extern void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);

int Vec3_ApproachHorz(struct Vec3* out, struct Vec3* a, int maxStep)
{
    struct Vec3 diff;
    struct Vec3 scaled;
    int len;
    Vec3_Sub(&diff, out, a);
    diff.y = 0;
    len = Vec3_HorzLen(&diff);
    if (maxStep != 0) {
        if (len == 0 || len <= maxStep) {
            out->x = a->x;
            out->z = a->z;
            return 1;
        }
        {
            int s = _ZN4cstd4fdivEii(maxStep, len);
            Vec3_MulScalar(&scaled, &diff, s);
            SubVec3(out, &scaled, out);
        }
    } else {
        if (len == 0) return 1;
    }
    return 0;
}
