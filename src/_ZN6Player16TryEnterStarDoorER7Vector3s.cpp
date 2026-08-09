//cpp
// @symbol _ZN6Player16TryEnterStarDoorER7Vector3s
/* recovered: named members + shared header, real C++ method
 *
 * Only from the walking, waiting or crouching states. Takes control, records
 * the door position in the 0x750 anchor and derives the approach point at
 * 0x744 from it, and turns the player to face the door. The one-shot guarded
 * by bit 0 of data_ov002_0210e164 registers the offset vector once.
 */
#include "Player.h"
extern "C" {
extern Player::State data_ov002_0211013c;   /* walk */
extern Player::State data_ov002_02110154;   /* wait */
extern Player::State data_ov002_0211043c;   /* crouch */
extern int data_ov002_0210e164;
extern int data_ov002_0210f344[3];
extern int data_ov002_0210f308;
extern void func_020072c0(void);

extern int _ZN6Player17SetNoControlStateEhih(void *c, unsigned char a, int b, unsigned char d);
extern void func_020731dc(int a, int b, void **node);
extern void Vec3_RotateYAndTranslate(void *out, void *trans, short angle, int *vec);
extern short Vec3_HorzAngle(const void *v0, const void *v1);
}

int Player::TryEnterStarDoor(Vector3 & pos_, short kind)
{
    int *pos = (int *)&pos_;
    if (IsState(data_ov002_0211013c) ||
        IsState(data_ov002_02110154) ||
        IsState(data_ov002_0211043c)) {
        if (_ZN6Player17SetNoControlStateEhih(this, 0xa, -1, 0)) {
            unk_750 = pos[0];
            unk_754 = pos[1];
            unk_758 = pos[2];
            unk_69e = kind;
            mStateArg = 0;
            if (!(data_ov002_0210e164 & 1)) {
                data_ov002_0210f344[0] = 0;
                data_ov002_0210f344[1] = 0;
                data_ov002_0210f344[2] = 0x64000;
                func_020731dc((int)data_ov002_0210f344, (int)func_020072c0, (void **)&data_ov002_0210f308);
                data_ov002_0210e164 |= 1;
            }
            Vec3_RotateYAndTranslate(&unk_744, &unk_750, unk_69e, data_ov002_0210f344);
            mAngleY = Vec3_HorzAngle(pos, &mPosX) + 0x8000;
            return 1;
        }
    }
    return 0;
}
