//cpp
// @symbol _ZN6Player20St_InYoshiMouth_MainEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "Player.h"
typedef int s32;
typedef short s16;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;


extern "C" {
extern int func_ov002_020d5cec(char *c);
extern void Vec3_RotateYAndTranslate(Vector3 *res, Vector3 *trans, short angY, Vector3 *add);
extern void _ZN6Player11ChangeStateERNS_5StateE(char *c, void *s);
extern int _ZN6Player12FinishedAnimEv(char *c);
extern void Player_AdvanceAnims(char *c);
extern void _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *thiz, void *v, u32 a, int b, u32 cc, u32 d, u32 e);

extern int data_ov002_02110094[];
}

int Player::St_InYoshiMouth_Main()
{
    switch (mStateStep) {
    case 0:
        break;
    case 1:
        {
            char *r4 = *(char **)((char *)&mEatingPlayer);
            u32 t;
            int d;
            if (r4 != 0 && *(char **)(r4 + 0x360) != ((char *)this)) {
                if (func_ov002_020d5cec(((char *)this)))
                    return 1;
            }
            {
                int *p = (int *)(int)(((long long)(int)(r4 + 0x5c)));
                mPosX = p[0];
                mPosY = p[1];
                mPosZ = p[2];
            }
            d = func_ov002_020beb38(((char *)this));
            t = *(u16 *)(r4 + 0x6c6);
            if (*(u8 *)(r4 + 0x709) == 0 && *(u8 *)(r4 + 0x708) == 0) {
                t = (u16)(t - d);
                *(u8 *)(((long long)(int)((char *)&unk_6e6))) += d;
                if ((s16)t < 0)
                    t = 0;
            }
            if (t <= 1 && unk_6e6 >= 0x1e) {
                Vector3 res;
                Vector3 addv;
                Vector3 hv;
                addv.x = 0;
                addv.y = 0x64000;
                addv.z = 0x64000;
                Vec3_RotateYAndTranslate(&res, (Vector3 *)(r4 + 0x5c), *(s16 *)(r4 + 0x8e), &addv);
                hv.x = res.x;
                hv.y = res.y;
                hv.z = res.z;
                _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(r4, &hv, 1, 0, 0, (*(int *)(r4 + 8) + 1) & 0xff, 1);
                _ZN6Player11ChangeStateERNS_5StateE(((char *)this), data_ov002_0211019c);
                *(int *)(r4 + 0x360) = 0;
                mEatingPlayer = 0;
                return 1;
            }
            *(u16 *)(r4 + 0x6c6) = (s16)t;
            break;
        }
    case 2:
        if (mStateTimer == 0)
            *(int *)(((long long)(int)((char *)&mBodyClsnFlags))) |= 0x2000;
        if (_ZN6Player12FinishedAnimEv(((char *)this))) {
            mStateStep = 3;
            mHurtDamage = 1;
            _ZN6Player11ChangeStateERNS_5StateE(((char *)this), data_ov002_02110094);
        }
        break;
    case 3:
        {
            int d = func_ov002_020beb38(((char *)this));
            *(u16 *)(((long long)(int)((char *)&mStateTimer))) -= (d << 2);
            mStateWork = d;
            if ((s16)mStateTimer <= 0) {
                mStateTimer = 0;
                mOpacity = 0x1f;
                _ZN6Player11ChangeStateERNS_5StateE(((char *)this), data_ov002_0211007c);
            }
        }
        break;
    }
    Player_AdvanceAnims(((char *)this));
    return 1;
}
