extern void Vec3_Sub(void* out, void* a, void* b);
extern int LenVec3(void* v);
extern char* _ZN5Actor13ClosestPlayerEv(void* c);
extern void func_ov034_021125b8(void* c, int i);
extern unsigned char DecIfAbove0_Byte(unsigned char* p);
extern int RandomIntInternal(int* seed);
extern short _ZN5Actor18HorzAngleToCPlayerEv(void* c);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void* c);
extern short Vec3_HorzAngle(const void* a, const void* b);
extern void _Z14ApproachLinearRiii(int* p, int a, int b);

extern int RNG_STATE[];
extern unsigned char data_ov034_0211433c[];
extern int data_ov034_02114488[];

struct Vec3 { int x, y, z; };

void func_ov034_02112348(char* c)
{
    struct Vec3 v;
    int len;
    char* player;

    Vec3_Sub(&v, c + 0x5c, c + 0x8cc);
    len = LenVec3(&v);
    player = _ZN5Actor13ClosestPlayerEv(c);
    if (*(int*)(player + 0x60) > *(int*)(c + 0x60) + 0x12c000) {
        func_ov034_021125b8(c, 1);
        return;
    }

    if (DecIfAbove0_Byte((unsigned char*)(c + 0x8da)) == 0) {
        int r = (unsigned)RandomIntInternal(RNG_STATE) >> 0x1b;
        if (r & 1) {
            *(short*)(c + 0x8d8) = (short)((r & 0xf) << 0xc);
        } else {
            *(short*)(c + 0x8d8) = _ZN5Actor18HorzAngleToCPlayerEv(c);
        }
        *(unsigned char*)(c + 0x8da) = data_ov034_0211433c[*(unsigned char*)(c + 0x8db)];
    }

    if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x708) || len > 0x5dc000) {
        *(short*)(c + 0x8d8) = Vec3_HorzAngle(c + 0x5c, c + 0x8cc);
        *(unsigned char*)(c + 0x8da) = data_ov034_0211433c[*(unsigned char*)(c + 0x8db)];
    }
    _Z14ApproachLinearRiii((int*)(c + 0x98), data_ov034_02114488[0], 0x1000);
}
