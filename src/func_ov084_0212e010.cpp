//cpp
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

typedef struct Vec3 { int x, y, z; } Vec3;

extern "C" {
int ApproachLinear_s(short* val, short target, short step);
int ApproachLinear_i(int* val, int target, int step);
void func_ov084_0212d42c(void* self);
int _ZN9Animation8FinishedEv(void* anim);
void func_02012694(unsigned int a, char* p);
void func_ov084_0212d2dc(char* c);
void* _ZN5Actor10FindWithIDEj(unsigned int id);
void _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(void* self, Vec3* pos, unsigned int a, int b, short c);
void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, Vec3* pos, void* p, int c, int d);
void _ZN5Actor24KillAndTrackInDeathTableEv(void* self);
void func_ov084_0212d560(void* self);
void _ZN5Actor17TrackInDeathTableEv(void* self);
int _ZN5Actor13DistToCPlayerEv(void* self);
void func_0201267c(unsigned int a, char* p);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void* anim, void* file, int idx, int speed, unsigned int flags);
int IsStarCollectedInCurLevel(unsigned int flag);
void SetStarMarker(int i, void* self, int v);
int _ZN5Actor13ClosestPlayerEv(void* self);
short Vec3_HorzAngle(Vec3* a, Vec3* b);
}

extern void* data_ov084_02130e14[];
extern void* data_ov084_02130e04[];

extern "C" void func_ov084_0212e010(char* self)
{
    Vec3 buf1;
    Vec3 buf2;
    char* other;
    int dist;
    int b;

    if (*(u8*)(self + 0x21d) != 0) {
        short* hp = (short*)(((long long)(int)(self + 0x8e)) & 0xFFFFFFFFFFFFFFFFLL);
        *hp = (s16)(*hp + *(s16*)(self + 0x218));
        ApproachLinear_s((short*)(self + 0x218), 0, 0xc8);
        func_ov084_0212d42c(self);
        if (_ZN9Animation8FinishedEv(self + 0x160) == 0)
            return;
        (*(u8*)(((int)self + 0x21d) & 0xFFFFFFFFFFFFFFFF))--;
        if (*(u8*)(self + 0x21d) != 0)
            return;
        func_02012694(0x11f, self + 0x74);
        *(int*)(self + 0x228) = 0;
        *(int*)(self + 0x224) = *(int*)(self + 0x228);
        return;
    }

    func_ov084_0212d2dc(self);
    if (ApproachLinear_i((int*)(self + 0x204), 0, *(int*)(self + 0x214)) == 0)
        return;
    {
        u32* p0 = (u32*)(((long long)(int)(self + 0xb0)) & 0xFFFFFFFFFFFFFFFFLL);
        u32* p1 = (u32*)(((long long)(int)(self + 0x18c)) & 0xFFFFFFFFFFFFFFFFLL);
        *p0 &= ~0x10000000;
        *p1 |= 1;
    }

    if (*(u8*)(self + 0x21c) != 0) {
        *(u8*)(self + 0x21c) = 0;
        b = 0;
        if (*(u16*)(self + 0xc) == 0xfb)
            b = 1;
        if (b != false) {
            other = (char*)_ZN5Actor10FindWithIDEj(*(u32*)(self + 0x1f0));
            if (other == 0)
                return;
            (*(u8*)(((int)other + 0x21a) & 0xFFFFFFFFFFFFFFFF))--;
            if (*(u8*)(self + 0x21e) != 0)
                return;
            (*(u8*)(((int)other + 0x21b) & 0xFFFFFFFFFFFFFFFF))++;
            if (*(u8*)(self + 0x220) != 0) {
                buf1.x = *(int*)(self + 0x5c);
                buf1.y = *(int*)(self + 0x60);
                buf1.z = *(int*)(self + 0x64);
                _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(self, &buf1, 2, 0xa000, 0);
            }
            if (*(u8*)(other + 0x21b) == 5) {
                _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(0xb2, *(u8*)(self + 0x21f) | 0x40, (Vec3*)(self + 0x5c), 0, *(signed char*)(self + 0xcc), -1);
                _ZN5Actor24KillAndTrackInDeathTableEv(other);
                _ZN5Actor24KillAndTrackInDeathTableEv(self);
                return;
            }
            func_ov084_0212d560(self);
            if (*(int*)(self + 0x1e8) != 1) {
                _ZN5Actor24KillAndTrackInDeathTableEv(self);
                return;
            }
            _ZN5Actor17TrackInDeathTableEv(self);
            *(int*)(self + 0x1ec) = 4;
            return;
        }
        b = *(u16*)(self + 0xc) == 0xfd;
        if (b == false)
            return;
        if (*(u8*)(self + 0x21e) != 0)
            return;
        buf2.x = *(int*)(self + 0x5c);
        buf2.y = *(int*)(self + 0x60);
        buf2.z = *(int*)(self + 0x64);
        _ZN5Actor10SpawnCoinsERK7Vector3j5Fix12IiEs(self, &buf2, 1, 0xa000, 0);
        _ZN5Actor24KillAndTrackInDeathTableEv(self);
        return;
    }

    dist = _ZN5Actor13DistToCPlayerEv(self);
    b = *(u16*)(self + 0xc) == 0xfb;
    if (b != false) {
        other = (char*)_ZN5Actor10FindWithIDEj(*(u32*)(self + 0x1f0));
        if (other == 0)
            return;
    }
    if (*(u16*)(self + 0x100) <= 0x64)
        return;
    if (dist <= 0x64000)
        return;
    if (dist >= 0x320000)
        return;
    b = *(u16*)(self + 0xc) == 0xfb;
    if (b != false) {
        if (*(u8*)(other + 0x21a) >= 2)
            return;
    }
    b = *(u16*)(self + 0xc) == 0xfc;
    if (b != false)
        func_0201267c(0x104, self + 0x74);
    else
        func_0201267c(0x121, self + 0x74);
    b = 1;
    *(u8*)(self + 0x21c) = 1;
    if (*(u16*)(self + 0xc) != 0xfb)
        b = 0;
    if (b != false)
        (*(u8*)(((int)other + 0x21a) & 0xFFFFFFFFFFFFFFFF))++;
    b = *(u16*)(self + 0xc) == 0xfd;
    if (b != false) {
        *(int*)(self + 0x1ec) = 3;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x110, data_ov084_02130e14[1], 0x40000000, 0x1000, 0);
    } else {
        *(int*)(self + 0x1ec) = 2;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(self + 0x110, data_ov084_02130e04[1], 0x40000000, 0x1000, 0);
        *(int*)(self + 0x168) = 0;
    }
    b = *(u16*)(self + 0xc) == 0xfb;
    if (b != false) {
        if (*(signed char*)(other + 0x221) >= 0) {
            int v;
            if (IsStarCollectedInCurLevel(*(u8*)(self + 0x21f)) != 0)
                v = 3;
            else
                v = 2;
            SetStarMarker(*(signed char*)(other + 0x221), self, v);
            *(int*)(other + 0x1f4) = *(int*)(self + 4);
        }
    }
    dist = _ZN5Actor13ClosestPlayerEv(self);
    if (dist == 0)
        return;
    *(s16*)(self + 0x8e) = Vec3_HorzAngle((Vec3*)(self + 0x5c), (Vec3*)(dist + 0x5c));
}
