/* HOST COPY of src/_ZN10StarMarkerD1Ev.cpp -- StarMarker's non-deleting
 * destructor (vtable slot 16). The matched .cpp is a real C++ `~StarMarker(){}`,
 * so MSVC synthesizes the base/member destructor calls under ITS manglings
 * (??1Actor@@UAE@XZ, ??1Model@@QAE@XZ, ??1ShadowModel@@QAE@XZ,
 * ??1MovingCylinderClsnWithPos@@QAE@XZ), none of which resolve to the extern-C
 * ROM bodies (_ZN5ModelD1Ev and friends). Rather than alias four MSVC dtor
 * manglings, this spells the ROM D1's own four calls plus the vtable restore
 * directly -- exactly what the disassembly at 0x020e6cf4 does, and exactly D0
 * (src/_ZN10StarMarkerD0Ev.c) minus the final Memory::Deallocate. Byte-faithful
 * to the ROM's teardown; not a src edit and not undecompiled code.
 *
 * The symbol name matches the ROM's (_ZN10StarMarkerD1Ev), so the slot-16 thunk
 * in hal/actor_classes.cpp reaches it as a plain extern-C call and no __thiscall
 * dtor mangling enters.
 */
#include <cstddef>   /* size_t (pointer-width casts) */
extern "C" {
extern void *_ZTV12daStarBase_c[];   /* == _ZTV10StarMarker, 0x0210aab8 */
void _ZN11ShadowModelD1Ev(void *self);
void _ZN5ModelD1Ev(void *self);
void _ZN25MovingCylinderClsnWithPosD1Ev(void *self);
void *_ZN5ActorD2Ev(void *self);

int *_ZN10StarMarkerD1Ev(int *t)
{
    t[0] = (int)(size_t)_ZTV12daStarBase_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x164);
    _ZN5ModelD1Ev((char *)t + 0x114);
    _ZN25MovingCylinderClsnWithPosD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
}  /* extern "C" */
