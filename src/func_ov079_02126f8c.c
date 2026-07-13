//cpp
typedef int Fix12;
struct Vector3 { int x, y, z; };
struct Vector3_16 { short x, y, z; };
struct Actor;
struct Platform {
    int UpdateKillByMegaChar(short, short, short, Fix12);
    int IsClsnInRange(Fix12, Fix12);
};
struct Actor_s {
    static Actor* FindWithID(unsigned int);
    Actor* ClosestPlayer();
    static Actor* Spawn(unsigned int, unsigned int, const Vector3&, const Vector3_16*, int, int);
};
extern "C" {
Fix12 Vec3_HorzDist(const Vector3* a, const Vector3* b);
short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
int AngleDiff(int a, int b);
}
int Platform::UpdateKillByMegaChar(short, short, short, Fix12);
Actor* Actor_s::FindWithID(unsigned int);
Actor* Actor_s::ClosestPlayer();
Actor* Actor_s::Spawn(unsigned int, unsigned int, const Vector3&, const Vector3_16*, int, int);
int Platform::IsClsnInRange(Fix12, Fix12);

extern "C" int func_ov079_02126f8c(char* c) {
    Platform* self = (Platform*)c;
    if (self->UpdateKillByMegaChar(0x2000, 0, 0, 0xc8000))
        return 1;
    if (!Actor_s::FindWithID(*(unsigned int*)(c + 0x320))) {
        Actor* p = ((Actor_s*)c)->ClosestPlayer();
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
            Actor* spawned = Actor_s::Spawn(0xde, 0, pos, (Vector3_16*)(c + 0x8c), *(signed char*)(c + 0xcc), -1);
            *(int*)(c + 0x320) = *(int*)((char*)spawned + 4);
            *(int*)((char*)spawned + 0x3dc) = (int)c;
        }
    }
    self->IsClsnInRange(0, 0);
    return 1;
}
