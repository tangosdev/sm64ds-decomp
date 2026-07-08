typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;

typedef struct Vector3 { int x, y, z; } Vector3;

extern int func_ov078_02123804(char* c);
extern void func_ov078_02123864(char* c);
extern void func_ov002_020db54c(int a, int b, int cc, int d);
extern s16 Vec3_HorzAngle(const Vector3* v0, const Vector3* v1);
extern int ApproachAngle(void* dst, int target, int a, int b, int c);
extern int Vec3_Dist(const Vector3* a, const Vector3* b);
extern void func_02012694(int id, void* v);
extern void func_ov078_02125c48(char* c, void* p);

extern u8 STAR_ID;
extern void* data_ov078_021270cc;

int func_ov078_02123d3c(char* c)
{
    if (func_ov078_02123804(c) == 1) {
        func_ov078_02123864(c);
        if (*(int*)(c + 0x494) != 0) {
            func_ov002_020db54c(*(int*)(c + 0x494), 0, 0x50000, *(s16*)(c + 0x8e));
            *(int*)(c + 0x430) = *(int*)(c + 0x494);
            *(int*)(c + 0x494) = 0;
        }
        return 1;
    }

    Vector3 v;
    v.x = *(int*)(c + 0x4d4);
    v.y = *(int*)(c + 0x4d8);
    v.z = *(int*)(c + 0x4dc);

    if (STAR_ID != 1) {
        v.x = *(int*)(c + 0x5c);
        v.y = *(int*)(c + 0x60);
        v.z = *(int*)(c + 0x64);
        *(s16*)(((int)c + 0x94) & 0xFFFFFFFFFFFFFFFF) += 0x1000;
        *(int*)(c + 0x98) = 0;
    } else {
        s16 a = Vec3_HorzAngle((Vector3*)(c + 0x5c), &v);
        ApproachAngle(c + 0x94, a, 5, 0x1000, 0x100);
    }

    *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);

    if (Vec3_Dist((Vector3*)(c + 0x5c), &v) > 0x1e000) {
        if (*(u8*)(c + 0x505) == 0) {
            func_02012694(0x12d, c + 0x74);
            *(u8*)(c + 0x505) = 0xf;
        }
    } else {
        if (*(u8*)(c + 0x505) == 0) {
            *(u8*)(c + 0x505) = 0x14;
            func_02012694(0x12f, c + 0x74);
        }
    }

    if (*(u16*)(c + 0x100) == 0)
        func_ov078_02125c48(c, &data_ov078_021270cc);
    return 1;
}
