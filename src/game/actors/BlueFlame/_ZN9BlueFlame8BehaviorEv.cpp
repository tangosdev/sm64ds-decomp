//cpp
// @symbol _ZN9BlueFlame8BehaviorEv

#include "BlueFlame.h"
#include "Player.h"

extern "C" {
u8 DecIfAbove0_Byte(u8 *value);
u32 func_020228dc(Fix12i x, Fix12i y, Fix12i z);
u32 func_0202293c(Fix12i x, Fix12i y, Fix12i z);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    u32 id, Fix12i x, Fix12i y, Fix12i z);
}

int BlueFlame::Behavior()
{
    struct Vector3 pos;
    struct Vector3 particlePos;
    struct Vector3 dust;

    pos.x = mFlamePos.x;
    pos.y = mFlamePos.y;
    pos.z = mFlamePos.z;
    pos.y += 0x32000;

    if (DecIfAbove0_Byte(&mDisappearTimer) != 0) {
        mdCcAc_c.Clear();
        if (mDisappearTimer == 1) {
            ((int *)&dust)[0] = ((int *)&pos)[0];
            ((int *)&dust)[1] = ((int *)&pos)[1];
            ((int *)&dust)[2] = ((int *)&pos)[2];
            DisappearPoofDustAt(dust);
        }
        return 1;
    }

    mPosX = mFlamePos.x;
    mPosY = mFlamePos.y;
    mPosZ = mFlamePos.z;
    mFlags &= ~0xc0000;

    if ((int)(actorID == 0x13d) != 0)
        func_020228dc(pos.x, pos.y, pos.z);
    else
        func_0202293c(pos.x, pos.y, pos.z);

    if ((int)((mFlags & 0x20000) != 0) != 0) {
        mDisappearTimer = 0x1e;
        particlePos.x = mFlamePos.x;
        particlePos.y = mFlamePos.y;
        particlePos.z = mFlamePos.z;
        particlePos.y -= 0x1b000;
        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
            0x9f, ((int *)&particlePos)[0], particlePos.y, particlePos.z);
    }

    {
        int id = mdCcAc_c.otherOwner;
        if (id != 0) {
            dActor_c *actor = FindWithID(id);
            if (actor != 0) {
                if ((int)(actor->actorID == 0xbf) != 0) {
                    if ((mdCcAc_c.hitFlags & 0x8000) == 0) {
                        ((Player *)actor)->Burn();
                    }
                }
            }
        }
    }

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    return 1;
}
