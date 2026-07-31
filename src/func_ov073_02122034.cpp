//cpp
// @symbol func_ov073_02122034
/* recovered: shared common types */
#include "common.h"

extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
extern int func_02012694(int, const Vector3&);
}
extern "C" int func_ov073_02122034(char* c);
int func_ov073_02122034(char* c) {
    Vector3 pos;
    int t;
    *(short*)(c + 0x332) = 0;
    ((int*)&pos)[0] = *(int*)(c + 0x5c);
    ((int*)&pos)[1] = *(int*)(c + 0x60);
    ((int*)&pos)[2] = *(int*)(c + 0x64);
    ((int*)&pos)[0] = *(int*)(c + 0x5c);
    t = *(int*)(c + 0x60);
    ((int*)&pos)[1] = t;
    ((int*)&pos)[2] = *(int*)(c + 0x64);
    ((int*)&pos)[1] = t - 0x12c000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8b, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2]);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x8c, ((int*)&pos)[0], ((int*)&pos)[1], ((int*)&pos)[2]);
    func_02012694(0x172, *(Vector3*)(c + 0x74));
    return 1;
}