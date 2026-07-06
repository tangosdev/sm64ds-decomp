//cpp
typedef unsigned char u8;
typedef short s16;
struct Vector3 {
    int x, y, z;
    Vector3() {}
    Vector3(const Vector3& o) { x = o.x; y = o.y; z = o.z; }
    ~Vector3() {}
};
extern "C" {
void* _ZN5Actor13ClosestPlayerEv(void* c);
int _ZN5Actor18GetBitInDeathTableEv(void* c);
int _ZN5Actor18HorzAngleToCPlayerEv(void* c);
int _ZN5Actor13DistToCPlayerEv(void* c);
void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void* c, Vector3 v, unsigned int n, int f, short s);
void _ZN5Actor17TrackInDeathTableEv(void* c);
unsigned char DecIfAbove0_Byte(unsigned char* p);
int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void* c, int a, int b);

int func_ov091_02133738(char* c)
{
    void* player = _ZN5Actor13ClosestPlayerEv(c);
    if (*(u8*)(c + 0x31e) == 3
        && _ZN5Actor18GetBitInDeathTableEv(c) == 0
        && *(int*)((char*)player + 0x60) < *(int*)(c + 0x60) + 0x64000) {
        int angle = _ZN5Actor18HorzAngleToCPlayerEv(c);
        if (_ZN5Actor13DistToCPlayerEv(c) > 0x190000) {
            *(int*)(c + 0x324) = 0;
        } else {
            s16 diff = (s16)(angle - *(s16*)(c + 0x328));
            int val = *(int*)(c + 0x324);
            int* p;
            if ((val > 0 && diff < -100) || (val < 0 && diff > 100))
                *(int*)(c + 0x324) = 0;
            p = (int*)(((int)c + 0x324) & 0xFFFFFFFFFFFFFFFF);
            *p = *p + diff;
            {
                int a = *(int*)(c + 0x324);
                if (a < 0) a = -a;
                if (a > 0x30000 && *(u8*)(c + 0x320) == 0
                    && _ZN5Actor18GetBitInDeathTableEv(c) == 0) {
                    Vector3 v;
                    v.x = *(int*)(c + 0x5c);
                    v.y = *(int*)(c + 0x60);
                    v.z = *(int*)(c + 0x64);
                    v.y += 0xc8000;
                    _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(c, v, 5, 0x5000, 0);
                    _ZN5Actor17TrackInDeathTableEv(c);
                }
            }
        }
        *(s16*)(c + 0x328) = angle;
    }
    DecIfAbove0_Byte((u8*)(c + 0x31f));
    _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0, 0);
    return 1;
}
}
