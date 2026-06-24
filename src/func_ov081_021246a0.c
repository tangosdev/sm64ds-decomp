typedef unsigned char u8;
typedef unsigned short u16;
typedef int Fix12i;

typedef struct { int x, y, z; } Vector3;

extern int _ZNK12WithMeshClsn10IsOnGroundEv(void* c);
extern void _Z14ApproachLinearRiii(int* ref, int target, int step);
extern int _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void* a, Vector3* v, unsigned n, Fix12i f, short s);
extern void func_ov081_02124134(void* c);
extern void _ZN5Actor24KillAndTrackInDeathTableEv(void* c);

int func_ov081_021246a0(char* c)
{
    Vector3 v;
    if (*(int*)(c + 0x41c) != 0 || _ZNK12WithMeshClsn10IsOnGroundEv(c + 0x150) != 0) {
        _Z14ApproachLinearRiii((int*)(c + 0x80), 0x1700, 0x50);
        _Z14ApproachLinearRiii((int*)(c + 0x88), 0x1700, 0x50);
        _Z14ApproachLinearRiii((int*)(c + 0x84), 0, 0x50);
        _Z14ApproachLinearRiii((int*)(c + 0x45c), -0x4000, 0x199);
        if (*(u16*)(c + 0x100) == 0) *(u16*)(c + 0x100) = 0x14;
        if (*(int*)(c + 0x84) < 0x50) {
            if (*(u8*)(c + 0x469) == 0) {
                v.x = *(int*)(c + 0x5c);
                v.y = *(int*)(c + 0x60);
                v.z = *(int*)(c + 0x64);
                _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, &v, *(u8*)(c + 0x10a) + 1, 0xa000, 0);
            }
            func_ov081_02124134(c);
            _ZN5Actor24KillAndTrackInDeathTableEv(c);
        }
    }
    return 1;
}
