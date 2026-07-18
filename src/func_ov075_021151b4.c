struct Vec3 { int x, y, z; };
struct Mtx43 { int m[12]; };

extern void Vec3_Asr(struct Vec3 *d, struct Vec3 *s, int sh);
extern void Matrix4x3_FromTranslation(struct Mtx43 *m, int x, int y, int z);
extern struct Mtx43 data_020a0e68;

struct Entry { char pad[0x118]; int x, y, z; char rest[0x34]; };

void func_ov075_021151b4(char *c, int idx)
{
    struct Entry *e = ((struct Entry *)(c + 0x920)) + idx;
    struct Vec3 *src = (struct Vec3 *)(((long long)(int)((char *)e + 0x118)) & 0xFFFFFFFFFFFFFFFFLL);
    struct Vec3 v, out;
    int y;

    v.x = src->x;
    y = src->y;
    v.y = y;
    v.z = src->z;
    v.y = y + 0x32000;
    Vec3_Asr(&out, &v, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, out.x, out.y, out.z);
    *(struct Mtx43 *)(c + 0x8d8) = data_020a0e68;
}
