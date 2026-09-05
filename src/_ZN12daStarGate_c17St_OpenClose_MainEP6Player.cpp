//cpp
// @symbol _ZN12daStarGate_c17St_OpenClose_MainEP6Player
#include "daStarGate_c.h"
#include "Player.h"

struct Camera;

extern "C" int _Z14ApproachLinearRiii(int &x, int a, int b);
extern "C" unsigned char DecIfAbove0_Byte(unsigned char *p);
extern "C" void func_02012694(int a, void *b, int c);
extern "C" void _ZN6Camera14GoBehindPlayerEj(Camera *self, unsigned int a);
extern "C" int func_ov100_02144fcc(void);
extern "C" void Vec3_RotateYAndTranslate(Vector3 *out, void *m, short angle, Vector3 *in);

extern unsigned char data_0209f250;
extern Camera *data_0209f318;
extern char data_020a0ebc;
extern Vector3 data_ov100_02148948;
extern "C" void ChangeArea(int areaID);

bool daStarGate_c::St_OpenClose_Main(Player *player)
{
    int r5;
    Vector3 vec;
    int gy, gz, vd;

    r5 = func_ov100_02144fcc();
    if (_Z14ApproachLinearRiii(mHorzSpeed, mTerminalVelocity, 0x8000) != 0) {
        if (mTerminalVelocity != 0) {
            if (mCloseDoorTimer == 0) {
                mCloseDoorTimer = 0x1c;
            } else if (DecIfAbove0_Byte(&mCloseDoorTimer) == 0) {
                mTerminalVelocity = 0;
                func_02012694(0x4f, &mCamSpacePosX, 0);
            } else if (mCloseDoorTimer == 8) {
                _ZN6Camera14GoBehindPlayerEj(data_0209f318, data_0209f250);
            }
        } else if (r5 != 0) {
            ChangeState(&ST_WAIT, player);
            {
                int t;
                if (mScaleZ < 0)
                    t = mAngleX;
                else
                    t = mAngleZ;
                t = (s8)t;
                player->mAreaId = t;
                ChangeArea(t);
            }
        }
    }

    vd = mHorzSpeed;
    gy = data_ov100_02148948.y;
    gz = data_ov100_02148948.z;
    data_ov100_02148948.x = vd;
    vec.x = vd;
    vec.z = gz;
    vec.y = gy;
    Vec3_RotateYAndTranslate((Vector3 *)&unk_0a4, &data_020a0ebc,
                             mAngleY, &vec);
    return true;
}
