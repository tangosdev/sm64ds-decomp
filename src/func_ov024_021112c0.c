// @symbol func_ov024_021112c0
/* recovered: shared common types */
#include "common.h"
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int, int, int, int);
extern void _ZN7fBase_c18MarkForDestructionEv(void *);
extern void _ZN5Event6SetBitEj(unsigned int);
extern void _ZN7Minimap19UpdateLevelSpecificEv(void);

void func_ov024_021112c0(char *c)
{
    struct Vector3 v;
    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    v.y += 0xfa000;
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x1f, v.x, v.y, v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x20, *(volatile int *)&v.x, *(volatile int *)&v.y, *(volatile int *)&v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x21, *(volatile int *)&v.x, *(volatile int *)&v.y, *(volatile int *)&v.z);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x22, *(volatile int *)&v.x, *(volatile int *)&v.y, *(volatile int *)&v.z);
    _ZN7fBase_c18MarkForDestructionEv(c);
    _ZN5Event6SetBitEj(0xe);
    _ZN7Minimap19UpdateLevelSpecificEv();
}
