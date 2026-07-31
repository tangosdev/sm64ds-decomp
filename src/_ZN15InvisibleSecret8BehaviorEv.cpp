//cpp
// @symbol _ZN15InvisibleSecret8BehaviorEv
/* recovered: named members + shared header, real C++ method */
#include "InvisibleSecret.h"
typedef unsigned char u8;

struct V3 { int x, y, z; };

extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned int id, int x, int y, int z);
extern void _ZN9ActorBase18MarkForDestructionEv(char *self);
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);

int InvisibleSecret::Behavior()
{
    struct V3 pos;

    if (unk_14c != 0) return 1;

    *(int *)((int)((char *)&mVertSpeed) & 0xFFFFFFFFFFFFFFFFLL) += unk_09c;
    if (mVertSpeed < unk_0a0)
        mVertSpeed = unk_0a0;
    *(int *)((int)((char *)&mPosY) & 0xFFFFFFFFFFFFFFFFLL) += mVertSpeed;

    switch (unk_14e) {
    case 0:
        if (mPosY < mStartPosY) {
            mPosY = mStartPosY;
            mVertSpeed = 0xf000;
            (*(u8 *)((int)((char *)&unk_14e) & 0xFFFFFFFFFFFFFFFFLL))++;
        }
        break;
    case 1:
        if (mPosY < mStartPosY) {
            _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xd2, mPosX, *(volatile int *)((char *)&mPosY), mPosZ);
            _ZN9ActorBase18MarkForDestructionEv(((char *)this));
        }
        break;
    }

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    if (unk_138 != 0) {
        char *other = _ZN5Actor10FindWithIDEj(unk_138);
        if (other != 0) {
            struct V3 *op = (struct V3 *)((int)(other + 0x5c) & 0xFFFFFFFFFFFFFFFFLL);
            int oy;
            pos.x = op->x;
            pos.y = oy = op->y;
            pos.z = op->z;
            pos.y = oy + (unk_148 + (mPosY - mStartPosY));
        }
    }
    Matrix4x3_FromTranslation((void *)((char *)&unk_0f0), pos.x >> 3, pos.y >> 3, (*(volatile int *)&pos.z) >> 3);
    return 1;
}
