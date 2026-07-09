//cpp
struct Vector3 { int x, y, z; };

extern "C" {
extern int SublevelToLevel(int i);
extern void Vec3_Sub(struct Vector3* out, struct Vector3* a, struct Vector3* b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern signed char data_0209f2f8;
extern signed char data_ov002_02111148;

void _ZN7Minimap21FixTHIPaintingRoomPosER7Vector3(struct Vector3* v)
{
    struct Vector3 p0, p1, out, out2;

    p0.x = (int)0xfee30000;
    p0.y = 0;
    p0.z = (int)0xff564000;
    p1.x = -0x1440000;
    p1.y = 0;
    p1.z = (int)0xff741000;

    if (SublevelToLevel(data_0209f2f8) != 0x1d) return;
    if (data_0209f2f8 != 5) return;
    if (data_ov002_02111148 != 2) return;

    if (v->z <= p0.z) {
        int factor;
        Vec3_Sub(&out, v, &p0);
        factor = 0xc00 - (_ZN4cstd4fdivEii(out.z, (int)0xffffd1a0) >> 12);
        v->x = p0.x + (int)(((long long)out.x * factor + 0x800) >> 12);
        v->z = p0.z + (int)(((long long)out.z * 0x200 + 0x800) >> 12);
    } else {
        if (v->x > p1.x) return;
        Vec3_Sub(&out2, v, &p1);
        v->x = p1.x + (int)(((long long)out2.x * 0x400 + 0x800) >> 12);
        v->z = p1.z + (int)(((long long)out2.z * 0xe00 + 0x800) >> 12);
    }
}
}
