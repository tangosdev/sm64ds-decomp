//cpp
typedef int Fix12;
typedef short s16;
typedef unsigned char u8;

struct Vector3 { int x, y, z; };

struct Actor {
    Actor* ClosestPlayer();
    static Actor* FindWithActorID(unsigned id, Actor* a);
};
struct Animation { void Advance(); };
struct CylinderClsn { void Clear(); void Update(); };
struct MovingCylinderClsnWithPos { void SetPosRelativeToActor(const Vector3& v); };

Actor* Actor::ClosestPlayer();
Actor* Actor::FindWithActorID(unsigned, Actor*);
void Animation::Advance();
void CylinderClsn::Clear();
void CylinderClsn::Update();
void MovingCylinderClsnWithPos::SetPosRelativeToActor(const Vector3&);

extern "C" {
extern int RandomIntInternal(int* seed);
extern s16 Vec3_HorzAngle(const Vector3* a, const Vector3* b);
extern Fix12 Vec3_HorzDist(const Vector3* a, const Vector3* b);
extern void func_ov060_02112434(void* t);
extern void func_ov060_02111a28(void* t);
extern void func_ov060_0211577c(void* t);
extern int data_0209e650;
extern char* data_0209f318;
}

extern "C" int _ZN6Bowser8BehaviorEv(char* self) {
    RandomIntInternal(&data_0209e650);
    *(int*)(self + 0x3a0) = (int)((Actor*)self)->ClosestPlayer();
    if (*(Actor**)(self + 0x3a0) != 0) {
        *(s16*)(self + 0x406) = Vec3_HorzAngle((Vector3*)(self + 0x5c), (Vector3*)((char*)*(Actor**)(self + 0x3a0) + 0x5c));
        *(int*)(self + 0x3ec) = Vec3_HorzDist((Vector3*)(self + 0x5c), (Vector3*)((char*)*(Actor**)(self + 0x3a0) + 0x5c));
    } else {
        *(s16*)(self + 0x406) = *(s16*)(self + 0x8e);
        *(int*)(self + 0x3ec) = ~0x80000000;
    }
    func_ov060_02112434(self);
    func_ov060_02111a28(self);
    *(s16*)(self + 0x94) = *(s16*)(self + 0x8e);
    *(int*)(self + 0x130) = *(int*)(self + 0x3f8);
    ((Animation*)(self + 0x124))->Advance();
    func_ov060_0211577c(self);
    *(char**)(data_0209f318 + 0x114) = self;
    ((CylinderClsn*)(self + 0x360))->Clear();
    Vector3 v;
    v.z = 0x50000;
    v.x = 0;
    v.y = 0;
    ((MovingCylinderClsnWithPos*)(self + 0x360))->SetPosRelativeToActor(v);
    ((CylinderClsn*)(self + 0x360))->Update();
    if (*(u8*)(self + 0x42b) != 0) {
        Actor* f = Actor::FindWithActorID(0x10d, 0);
        if (f == 0) *(u8*)(self + 0x42b) = 0;
    }
    return 1;
}
