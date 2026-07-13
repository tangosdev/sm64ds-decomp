//cpp
typedef unsigned char u8;
typedef signed char s8;
typedef short s16;
struct Vector3 { int x, y, z; };
struct Vector3_16 { s16 x, y, z; };
extern "C" {
extern int _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(void* self, void* a, void* b, unsigned c);
extern void* _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned a, unsigned b, const Vector3* pos, const Vector3_16* rot, int e, int f);
extern void* _ZN5Actor10FindWithIDEj(unsigned id);
extern int RandomIntInternal(void* seed);
extern int data_0209e650;
extern int func_ov064_02116d1c(void* thiz);

int _ZN5Bully8BehaviorEv(char* self) {
    int ret = _ZN5Enemy26UpdateKillByInvincibleCharER12WithMeshClsnR9ModelAnimj(
        self, self + 0x174, self + 0x110, 3);
    if (ret != 0) {
        if (ret == 2) {
            int y = *(int*)(self + 0x60);
            int yoff = 0x136000;
            int z = *(int*)(self + 0x64);
            int x = *(int*)(self + 0x5c);
            int sum = y + yoff;
            Vector3 pos;
            pos.x = x; pos.y = sum; pos.z = z;
            int r = RandomIntInternal(&data_0209e650);
            int ang = *(s16*)(self + 0x94);
            int yrot = ang + 0x8000 + (int)(((unsigned)r >> 8) & 0x3ff);
            Vector3_16 rot;
            rot.x = 0;
            rot.z = 0;
            rot.y = (s16)yrot;

            void* a = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0x120, 2, &pos, &rot, *(s8*)(self + 0xcc), -1);
            if (a) {
                *(int*)((char*)a + 0x98) = 0xa000;
                *(int*)((char*)a + 0xa4) = 0;
                *(int*)((char*)a + 0xa8) = 0x50000;
                *(int*)((char*)a + 0xac) = 0;
            }
            void* f = _ZN5Actor10FindWithIDEj(*(unsigned*)(self + 0x3fc));
            if (f) {
                *(u8*)(((long long)(int)((char*)f + 0x3fe)) & 0xFFFFFFFFFFFFFFFFLL) += 1;
            }
        }
        return 1;
    }
    return func_ov064_02116d1c(self);
}
}
