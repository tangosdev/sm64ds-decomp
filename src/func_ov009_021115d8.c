// @symbol func_ov009_021115d8
/* recovered: shared common types */
#include "common.h"
extern char* _ZN5Actor13ClosestPlayerEv(void* self);
extern int Vec3_Sub(struct Vector3* dst, void* a, void* b);
extern int Vec3_HorzLen(struct Vector3* v);
extern int func_0201267c(unsigned int a, void* b);
extern char* _ZN5Actor10FindWithIDEj(unsigned int id);
extern int _ZN9ActorBase18MarkForDestructionEv(char* t);
extern int RandomIntInternal(void* rng);
extern int data_0209e650;
void func_ov009_021115d8(char* c) {
    if (*(unsigned char*)(c+0x180) != 0) {
        char* p2 = _ZN5Actor13ClosestPlayerEv(c);
        if (p2 != 0) {
            struct Vector3 copy;
            struct Vector3 diff;
            int len;
            Vec3_Sub(&diff, c+0x5c, p2+0x5c);
            copy.x = diff.x;
            copy.y = diff.y;
            copy.z = diff.z;
            len = Vec3_HorzLen(&copy);
            if (len > 0x7d0000) return;
        }
        func_0201267c(0x6a, c+0x74);
    } else {
        char* p = _ZN5Actor10FindWithIDEj(*(unsigned int*)(c+0x178));
        if (p == 0) {
            _ZN9ActorBase18MarkForDestructionEv(c);
            return;
        }
        if (*(int*)(p+0x17c) != 3) return;
    }
    *(short*)(c+0x92) = 5000 - (unsigned int)RandomIntInternal(&data_0209e650) % 4000;
    *(int*)(c+0x174) = 0x28000;
    *(int*)(c+0x17c) = 3;
    *(int *)(((int)c + 0xb0)) &= ~0x10000;
}
