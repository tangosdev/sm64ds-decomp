//cpp
typedef unsigned char u8; typedef signed char s8; typedef short s16;
struct Vector3 { int x, y, z; }; struct Vector3_16 { s16 x, y, z; };
struct Actor {
  static Actor* Spawn(unsigned a, unsigned b, const Vector3& pos, const Vector3_16* rot, int e, int f);
  static Actor* FindWithID(unsigned id);
};
Actor* Actor::Spawn(unsigned, unsigned, const Vector3&, const Vector3_16*, int, int);
Actor* Actor::FindWithID(unsigned);
extern "C" { extern int RandomIntInternal(void* seed); extern int data_0209e650; extern int func_ov064_0211616c(void* thiz); }
extern "C" void func_ov064_02117220(char* self) {
    if (func_ov064_0211616c(self) == 0) return;
    int pz = *(int*)(self + 0x64);
    int py = *(int*)(self + 0x60) + 0x136000;
    int px = *(int*)(self + 0x5c);
    Vector3 pos;
    pos.x = px;
    pos.y = py;
    pos.z = pz;
    int r = RandomIntInternal(&data_0209e650);
    Vector3_16 rot;
    s16 ang = (s16)(*(s16*)(self + 0x94) + 0x8000 + (((unsigned)r >> 8) & 0x3ff));
    rot.x = 0; rot.z = 0; rot.y = ang;
    Actor* a = Actor::Spawn(0x120, 2, pos, &rot, *(s8*)(self + 0xcc), -1);
    if (a) {
        *(int*)((char*)a + 0x98) = 0xa000;
        *(int*)((char*)a + 0xa4) = 0;
        *(int*)((char*)a + 0xa8) = 0x50000;
        *(int*)((char*)a + 0xac) = 0;
    }
    Actor* f = Actor::FindWithID(*(unsigned*)(self + 0x3fc));
    if (f) {
        u8* p = (u8*)(((long long)(int)((char*)f + 0x3fe)) & 0xFFFFFFFFFFFFFFFFLL);
        *p = (u8)(*p + 1);
    }
}
