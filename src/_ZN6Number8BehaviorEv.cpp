//cpp
#include "types.h"
// @symbol _ZN6Number8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "Number.h"
struct V3 { int x, y, z; };

extern "C" {
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN7fBase_c18MarkForDestructionEv(char *self);
extern char *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
}

int Number::Behavior()
{
    struct V3 pos;

    if (unk_14c != 0) return 1;

    *(int *)((int)((char *)&mVertSpeed)) += mVertAccel;
    if (mVertSpeed < mTerminalVelocity)
        mVertSpeed = mTerminalVelocity;
    *(int *)((int)((char *)&mPosY)) += mVertSpeed;

    switch (unk_14e) {
    case 0:
        if (mPosY < mStartPosY) {
            mPosY = mStartPosY;
            mVertSpeed = 0xf000;
            (*(u8 *)((int)((char *)&unk_14e)))++;
        }
        break;
    case 1:
        if (mPosY < mStartPosY) {
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, *(volatile int *)((char *)&mPosY), mPosZ);
            _ZN7fBase_c18MarkForDestructionEv(((char *)this));
        }
        break;
    }

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    if (unk_138 != 0) {
        char *other = _ZN8dActor_c10FindWithIDEj(unk_138);
        if (other != 0) {
            struct V3 *op = (struct V3 *)((int)(other + 0x5c));
            int oy;
            pos.x = op->x;
            pos.y = oy = op->y;
            pos.z = op->z;
            pos.y = oy + (unk_148 + (mPosY - mStartPosY));
        }
    }
    /* 0x0f0 is +0x1c inside the Model at 0x0d4 -- its mat4x3. The cartridge's own
       ~Number proves the Model's extent; see tools/dtor_members.py. */
    Matrix4x3_FromTranslation((void *)((char *)&mModel.mat4x3), pos.x >> 3, pos.y >> 3, (*(volatile int *)&pos.z) >> 3);
    return 1;
}
