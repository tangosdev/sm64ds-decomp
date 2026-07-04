struct Vector3 { int x, y, z; };

extern int Vec3_Dist(void* a, void* b);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int cc, void* v, unsigned int d);
extern int func_ov077_02126300(char* c);
extern void func_ov077_02126d5c(char* c, void* p);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void* p);
extern short Vec3_HorzAngle(void* a, void* b);
extern void _Z14ApproachLinearRsss(short* a, short b, short cc);
extern void* _ZN5Actor13ClosestPlayerEv(char* c);
extern int _ZN6Player12GetHurtStateEv(void* p);

extern char data_ov077_02127d18[];
extern char data_ov077_02127cf8[];
extern char data_ov077_02127d08[];

int func_ov077_02126ad0(char* c)
{
    int dist;
    char* player;
    struct Vector3 pp;

    dist = Vec3_Dist(c + 0x5c, c + 0x404);
    *(unsigned int*)(c + 0x428) = _ZN5Sound8PlayLongEjjjRK7Vector3j(*(unsigned int*)(c + 0x428), 3, 0x186, c + 0x74, 0);

    if (func_ov077_02126300(c) != 0) {
        func_ov077_02126d5c(c, data_ov077_02127d18);
        return 1;
    }

    if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x184) != 0) {
        *(int*)(c + 0x5c) = *(int*)(c + 0x410);
        *(int*)(c + 0x60) = *(int*)(c + 0x414);
        *(int*)(c + 0x64) = *(int*)(c + 0x418);
    }

    if (dist > 0x1f4000) {
        *(short*)(c + 0x420) = Vec3_HorzAngle(c + 0x5c, c + 0x404);
        if (*(unsigned short*)(c + 0x100) < 0x14)
            *(unsigned short*)(c + 0x100) = 0x14;
        _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x420), 0x400);
    }
    _Z14ApproachLinearRsss((short*)(c + 0x94), *(short*)(c + 0x420), 0x100);

    if (*(unsigned short*)(c + 0x100) < 0x64)
        *(int*)(c + 0x39c) = 0x1000 / (0x64 - *(unsigned short*)(c + 0x100));

    if (*(unsigned short*)(c + 0x100) == 0) {
        func_ov077_02126d5c(c, data_ov077_02127cf8);
        return 1;
    }

    if (*(unsigned short*)(c + 0x426) != 0)
        return 1;

    player = (char*)_ZN5Actor13ClosestPlayerEv(c);
    if (player != 0) {
        struct Vector3* src = (struct Vector3*)(((long)(player + 0x5c)) & 0xFFFFFFFFFFFFFFFF);
        pp.x = src->x;
        pp.y = src->y;
        pp.z = src->z;
        if (Vec3_Dist(c + 0x404, &pp) < 0x3e8000
            && *(unsigned char*)(player + 0x6f9) == 0
            && *(unsigned char*)(player + 0x703) == 0
            && _ZN6Player12GetHurtStateEv(player) < 0) {
            func_ov077_02126d5c(c, data_ov077_02127d08);
            return 1;
        }
    }
    return 1;
}
