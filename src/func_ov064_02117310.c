//cpp
// NONMATCHING: different op / idiom (div=15). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
typedef int Fix12;
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;

struct Vector3 { int x, y, z; };
struct Vector3_16 { s16 x, y, z; };
struct WithMeshClsn;
struct ModelAnim;

struct Actor {
    static Actor* Spawn(unsigned a, unsigned b, const Vector3& pos,
                        const Vector3_16* rot, int e, int f);
    static Actor* FindWithID(unsigned id);
};
Actor* Actor::Spawn(unsigned, unsigned, const Vector3&, const Vector3_16*, int, int);
Actor* Actor::FindWithID(unsigned);

struct Enemy {
    int UpdateKillByInvincibleChar(WithMeshClsn& a, ModelAnim& b, unsigned c);
};
int Enemy::UpdateKillByInvincibleChar(WithMeshClsn&, ModelAnim&, unsigned);

extern "C" {
extern int RandomIntInternal(void* seed);
extern int RNG_STATE;
extern int func_ov064_02116d1c(void* thiz);
}

extern "C" int func_ov064_02117310(char* self) {
    int ret = ((Enemy*)self)->UpdateKillByInvincibleChar(
        *(WithMeshClsn*)(self + 0x174), *(ModelAnim*)(self + 0x110), 3);
    if (ret != 0) {
    if (ret == 2) {
        int py = *(int*)(self + 0x60);
        int pz = *(int*)(self + 0x64);
        Vector3 pos;
        pos.x = *(int*)(self + 0x5c);
        pos.y = py + 0x136000;
        pos.z = pz;
        int r = RandomIntInternal(&RNG_STATE);
        Vector3_16 rot;
        rot.y = (s16)(*(s16*)(self + 0x94) + 0x8000 + (((unsigned)r >> 8) & 0x3ff));
        rot.x = 0;
        rot.z = 0;
        Actor* a = Actor::Spawn(0x120, 2, pos, &rot, *(s8*)(self + 0xcc), -1);
        if (a) {
            *(int*)((char*)a + 0x98) = 0xa000;
            *(int*)((char*)a + 0xa4) = 0;
            *(int*)((char*)a + 0xa8) = 0x50000;
            *(int*)((char*)a + 0xac) = 0;
        }
        Actor* f = Actor::FindWithID(*(unsigned*)(self + 0x3fc));
        if (f) (*(u8*)((char*)f + 0x3fe))++;
    }
    return 1;
    }
    return func_ov064_02116d1c(self);
}
