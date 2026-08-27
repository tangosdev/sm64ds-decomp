#pragma opt_propagation off
typedef unsigned char u8;
typedef short s16;
typedef int Fix12i;
typedef struct { int x, y, z; } Vector3;

extern void func_0203db0c(int idx, unsigned short val);
extern short ReadUnalignedShort(unsigned char *p);
extern Fix12i Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern s16 Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);

extern char data_0209f4a6[];
extern char data_0209f4a0[];
extern u8 data_020a0e40;
extern char data_0209f4ac[];
extern char data_0209f49c[];

int func_ov002_020bd4e0(char* self, unsigned char* data)
{
    Vector3 v;
    int y, z;
    int idx;

    func_0203db0c(0, 0);
    z = ReadUnalignedShort(data + 4) << 12;
    y = ReadUnalignedShort(data + 2) << 12;
    v.x = ReadUnalignedShort(data) << 12;
    v.y = y;
    v.z = z;

    if (Vec3_HorzDist((Vector3*)(self + 0x5c), &v) <= *(int*)(self + 0x98)) {
        *(int*)(self + 0x5c) = v.x;
        *(int*)(self + 0x64) = v.z;
        *(int*)(self + 0x98) = 0;
        return 1;
    } else {
        s16 ang = Vec3_HorzAngle((Vector3*)(self + 0x5c), &v);
        idx = *(u8*)(self + 0x6d8);
        *(short*)(data_0209f4a6 + idx * 0x18) = ang;
        s16 v6 = ReadUnalignedShort(data + 6);
        *(short*)(data_0209f4a0 + *(u8*)(self + 0x6d8) * 0x18) = v6;
        {
        int pi = *(u8*)(self + 0x6d8) * 0x18;
        *(u8*)(data_0209f4ac + pi) = 1;
        }
        *(unsigned short*)(data_0209f49c + *(u8*)(self + 0x6d8) * 0x18) = *(unsigned short*)(data_0209f49c + data_020a0e40 * 0x18) | 0x800;
        return 1;
    }
}
