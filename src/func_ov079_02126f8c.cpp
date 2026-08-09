//cpp
// @symbol func_ov079_02126f8c
// recovered name: daObjBkKillerdai_c_Behavior
/* recovered: shared common types, renamed to Class_Method */
/* daObjBkKillerdai_c::Behavior - recovered from vtable slot identity */
typedef int Fix12;
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };
struct Actor;
struct Platform {
    int UpdateKillByMegaChar(short, short, short, Fix12);
    int IsClsnInRange(Fix12, Fix12);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" int _ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(void *, short, short, short, Fix12);
extern "C" int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void *, Fix12, Fix12);

struct Actor_s {
    static Actor* FindWithID(unsigned int);
    Actor* ClosestPlayer();
    static Actor* Spawn(unsigned int, unsigned int, const Vector3&, const Vector3_16*, signed char, short);
};
/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
extern "C" Actor* _ZN5Actor10FindWithIDEj(unsigned int);
extern "C" Actor* _ZN5Actor13ClosestPlayerEv(void *);
extern "C" Actor* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(unsigned int, unsigned int, const Vector3&, const Vector3_16*, int, int);

extern "C" {
Fix12 Vec3_HorzDist(const Vector3* a, const Vector3* b);
short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
int AngleDiff(int a, int b);
}
int Platform::UpdateKillByMegaChar(short, short, short, Fix12);
Actor* Actor_s::FindWithID(unsigned int);
Actor* Actor_s::ClosestPlayer();
Actor* Actor_s::Spawn(unsigned int, unsigned int, const Vector3&, const Vector3_16*, signed char, short);
int Platform::IsClsnInRange(Fix12, Fix12);

extern "C" int func_ov079_02126f8c(char* c) {
    Platform* self = (Platform*)c;
    if (_ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(self, 0x2000, 0, 0, 0xc8000))
        return 1;
    if (!_ZN5Actor10FindWithIDEj(*(unsigned int*)(c + 0x320))) {
        Actor* p = _ZN5Actor13ClosestPlayerEv((Actor_s*)c);
        Fix12 dist = Vec3_HorzDist((Vector3*)(c + 0x5c), (Vector3*)((char*)p + 0x5c));
        short ang = Vec3_HorzAngle((Vector3*)(c + 0x5c), (Vector3*)((char*)p + 0x5c));
        if (AngleDiff(ang, *(short*)(c + 0x8e)) < 0x2000 && dist > 0x320000 && dist < 0x5dc000) {
            long long yll = *(int*)(c + 0x60);
            int z = *(int*)(c + 0x64);
            int x = *(int*)(c + 0x5c);
            int y = (int)yll + 0x96000;
            Vector3 pos;
            *(volatile int*)&pos.x = x;
            *(volatile int*)&pos.z = z;
            *(volatile int*)&pos.y = y;
            Actor* spawned = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16as(0xde, 0, pos, (Vector3_16*)(c + 0x8c), *(signed char*)(c + 0xcc), -1);
            *(int*)(c + 0x320) = *(int*)((char*)spawned + 4);
            *(int*)((char*)spawned + 0x3dc) = (int)c;
        }
    }
    _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(self, 0, 0);
    return 1;
}
