//cpp
// @symbol func_ov006_020c7860
/* recovered: shared common types */
#include "common.h"
struct C; typedef void (C::*PMF)();
extern "C" {
extern void _Z14ApproachLinearRiii(int &, int, int);

extern void AddVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern void func_ov006_020bfec0(void* a, char* b, short* d);
extern void func_ov006_020c76e0(char *c);
extern void _ZN9Animation7AdvanceEv(void *);
extern int data_ov006_0213b010;
extern int data_ov006_0213b018;
extern void *data_ov006_02141a40;
}
struct C { char pad[0x3c]; PMF m; };
extern "C" void func_ov006_020c7860(char *c);
extern "C" void func_ov006_020c7860(char *c) {
    _Z14ApproachLinearRiii(*(int*)(c + 0x24), data_ov006_0213b010, data_ov006_0213b018);
    AddVec3((struct Vector3*)(c + 0x14), (struct Vector3*)(c + 0x20), (struct Vector3*)(c + 0x14));
    { C *o = (C*)c; (o->*o->m)(); }
    func_ov006_020bfec0(*(void**)&data_ov006_02141a40, c + 0x14, (short*)(c + 0x36));
    func_ov006_020c76e0(c);
    _ZN9Animation7AdvanceEv(c + 0x9c);
}
