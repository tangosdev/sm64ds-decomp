//cpp
/* HOST COPY of src/func_ov002_020e4768.cpp -- one MSVC vtable slot-fold made
 * explicit.
 *
 * The matched TU poses Yoshi's head each frame with a virtual call on the head
 * ModelAnim at Player +0x160 (index 3 of the head array at +0x154). The ROM
 * spells it as the fifth declared virtual of the head's own class -- Itanium
 * slot 4, which for ModelAnim is Virtual10(Matrix4x3&), the "pose this model's
 * bones through the matrix I hand you" method (the neck bone the body just
 * updated). See include/ModelAnim.h: the ROM vtable is
 *   0 (~D1) 1 (~D0) 2 DoSetFile 3 UpdateVerts 4 Virtual10 5 Render 6 Virtual18.
 *
 * MSVC folds the two destructor slots into one, so the port's _ZTV9ModelAnim is
 *   0 dtor 1 DoSetFile 2 UpdateVerts 3 Virtual10 4 Render 5 Virtual18
 * (hal/cxxname_bridge.cpp fills it in that order on purpose, so the many other
 * matched TUs MSVC compiles dispatch to the right method). But that also means
 * the matched TU's "fifth virtual" call lands on MSVC slot 4 -- Render -- not
 * Virtual10. The head is then handed the neck matrix as a scale vector and
 * RENDERED an extra time, and its bones are never posed. The mouth/tongue bones
 * stay at rest, so func_ov002_020d6dac's tongue raycast collapses to the player
 * origin, hits the ground on frame one and forces the tongue press straight into
 * its retract step -- St_YoshiPower_Main never reaches common_tail, so the head
 * ModelAnim's Animation::Advance never runs and currFrame holds at 0.
 *
 * The vtable is kept ROM-shaped for every other consumer; this one consumer is
 * host-copied with the slot named outright: Virtual10(neckMatrix). Everything
 * else is byte-for-byte the matched code.
 */
#include "ModelAnim.h"

extern "C" {
extern unsigned int _ZNK6Player14GetBodyModelIDEjb(void* self, unsigned int idx, int flag);
extern void func_020167a4(void* p);
extern void func_ov002_020e640c(void* self);
extern void _ZN15ModelComponents21UpdateVertsUsingBonesEv(void* self);
extern int func_ov002_020becf4(void* self, unsigned int idx, int flag);
extern void MulMat4x3Mat4x3(void* dst, void* a, void* b);
extern int data_020a0e68[12];
}

typedef struct { int w[12]; } Mat43;

// PORT_HOST_ABI: the matched TU's fifth-virtual call is ROM slot 4 (ModelAnim::Virtual10) but MSVC folds the two destructor slots into one, so it would land on Render; Virtual10 named outright
extern "C" void func_ov002_020e4768(char* self)
{
    unsigned int mid;
    void* comp;
    int idx;

    mid = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
    comp = *(void**)(self + mid*4 + 0xdc);
    func_020167a4(comp);

    func_ov002_020e640c(self);

    mid = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
    comp = *(void**)(self + mid*4 + 0xdc);
    _ZN15ModelComponents21UpdateVertsUsingBonesEv((char*)comp + 8);

    idx = func_ov002_020becf4(self, *(unsigned int*)(self+8) & 0xff, 0);

    if (idx == 3) {
        unsigned int m2 = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
        int a1 = *(int*)((char*)(*(void**)(self + m2*4 + 0xdc)) + 0x14) + 0x2d0;
        /* ROM slot 4 == ModelAnim::Virtual10(Matrix4x3&): pose the head's bones
         * through the body's neck bone. Named explicitly so MSVC's folded vtable
         * cannot land it on Render. */
        ModelAnim* head = *(ModelAnim**)(self + 0x154 + idx * 4);
        head->ModelAnim::Virtual10(*(Matrix4x3*)a1);
    }

    {
        int hasFlag = (*(int*)(self + 0x358) != 0);
        if (hasFlag) {
            mid = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
            comp = *(void**)(self + mid*4 + 0xdc);
            *(Mat43*)data_020a0e68 = *(Mat43*)((char*)comp + 0x1c);

            mid = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
            comp = *(void**)(self + mid*4 + 0xdc);
            MulMat4x3Mat4x3((char*)*(int*)((char*)comp + 0x14) + 0x2a0, data_020a0e68, data_020a0e68);

            *(Mat43*)(self + 0x58c) = *(Mat43*)data_020a0e68;
            *(void**)(self + 0xc8) = self + 0x58c;
        }
    }

    mid = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
    comp = *(void**)(self + mid*4 + 0xdc);
    *(Mat43*)data_020a0e68 = *(Mat43*)((char*)comp + 0x1c);

    mid = _ZNK6Player14GetBodyModelIDEjb(self, *(unsigned int*)(self+8) & 0xff, 0);
    comp = *(void**)(self + mid*4 + 0xdc);
    MulMat4x3Mat4x3((char*)*(int*)((char*)comp + 0x14) + 0x2d0, data_020a0e68, data_020a0e68);

    *(Mat43*)(self + 0x5ec) = *(Mat43*)data_020a0e68;
}
