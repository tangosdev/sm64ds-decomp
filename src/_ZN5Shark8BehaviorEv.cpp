//cpp
// @symbol _ZN5Shark8BehaviorEv

#include "Shark.h"

struct SharkCallbackOwner;
typedef void (SharkCallbackOwner::*SharkCallback)();

struct SharkBehaviorState {
    char pad_00[8];
    SharkCallback callback;
};

struct PathPtr {
    char pad_00[8];

    PathPtr();
    void FromID(u32 id);
    void GetNode(Vector3 &out, u32 index) const;
};

extern "C" {
u16 DecIfAbove0_Short(u16 *value);
void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
s32 LenVec3(Vector3 *value);
s16 Vec3_HorzAngle(Vector3 *a, Vector3 *b);
s16 Vec3_VertAngle(Vector3 *a, Vector3 *b);
void Matrix4x3_FromRotationY(void *matrix, s32 angle);
void Matrix4x3_ApplyInPlaceToRotationX(void *matrix, s16 angle);
void MulVec3Mat4x3(Vector3 *value, void *matrix, Vector3 *out);
void func_ov090_02133904(Shark *shark);
extern char data_020a0e68[];
}

void ApproachLinear(s16 &value, s16 target, s16 step);

int Shark::Behavior()
{
    DecIfAbove0_Short((u16 *)&unk_100);
    {
        SharkBehaviorState *state = *(SharkBehaviorState **)&mState;
        if (state->callback != 0) {
            (((SharkCallbackOwner *)this)->*(state->callback))();
        }
    }
    {
        PathPtr path;
        Vector3 node;
        Vector3 difference;
        Vector3 velocity;
        int distance;

        path.FromID(mPathID);
        path.GetNode(node, mPathNodeIdx);
        Vec3_Sub(&difference, (Vector3 *)&mPosX, &node);
        distance = LenVec3(&difference);
        if (distance == 0 || distance <= 0x258000) {
            mPathNodeIdx++;
            if (mPathNodeIdx >= mPathNodeCount)
                mPathNodeIdx = 0;
        }
        ApproachLinear(mPrevAngleY,
            Vec3_HorzAngle((Vector3 *)&mPosX, &node), 0x180);
        ApproachLinear(mPrevAngleX,
            Vec3_VertAngle((Vector3 *)&mPosX, &node), 0x40);
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
        velocity.y = velocity.x = velocity.z = 0;
        velocity.z = 0x14000;
        Matrix4x3_FromRotationY(data_020a0e68, mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(data_020a0e68, mPrevAngleX);
        MulVec3Mat4x3(&velocity, data_020a0e68, (Vector3 *)&unk_0a4);
    }
    {
        int speed = mVertSpeed + mVertAccel;
        int terminalVelocity = mTerminalVelocity;
        int unk = unk_0ac;
        if (speed >= terminalVelocity)
            terminalVelocity = speed;
        mVertSpeed = terminalVelocity;
        unk_0ac = unk;
    }
    UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    func_ov090_02133904(this);
    mdCcAcPos_c.Clear();
    {
        Player *player = ClosestPlayer();
        if (player != 0 && *((u8 *)player + 0x6fb) == 0)
            mdCcAcPos_c.Update();
    }
    return 1;
}
