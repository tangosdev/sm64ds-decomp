//cpp
// @symbol _ZN13dScGameOver_c8BehaviorEv
/* recovered: named members + real C++ method */
/* dScGameOver_c::Behavior() -- vtable slot 6. A two-state cursor FSM over the
 * yes/no prompt: state 0 reads the stylus box and the button, state 1 runs the
 * two countdowns down and commits -- continue (func_0202ae74) on "yes", or a
 * scene fade back out on "no". unk_094 gates all of it until the intro is in. */
#include "dScGameOver_c.h"

struct VT { void *v0, *v1, *v2, *v3, *v4; int (*m_14)(void *); };
struct Obj { VT *vt; };

extern "C" {
extern Obj *data_0209f5bc;

void func_ov003_020b060c(void *self);
int IsButtonInputValid(void);
void func_ov003_020b0730(void *self, int flag);
void func_02012790(int se);
void func_0202ae74(void);
void _ZN8dScene_c14StartSceneFadeEjjt(unsigned int, unsigned int, unsigned short);
void _ZN5Sound22StopLoadedMusic_Layer1Ej(unsigned int);

extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern unsigned char data_020a0dea[];
extern unsigned char data_020a0deb[];
extern int data_0208ee44;
}

s32 dScGameOver_c::Behavior()
{
    Obj *o = data_0209f5bc;

    if (o->vt->m_14(o) == 0)
        goto end;

    func_ov003_020b060c(this);

    if (unk_094 < 2)
        return 1;

    switch (unk_090) {
        case 0: goto state0;
        case 1: goto state1;
        default: goto end;
    }

state0:
        {
            unsigned char idx = data_020a0e40;
            int b = 0;
            if (data_020a0de8[idx * 4] != 0)
                b = (data_020a0de9[idx * 4] != 0);
            if (!b) {
                if (!IsButtonInputValid())
                    goto end;
            }
        }
        {
            unsigned char idx = data_020a0e40;
            if ((unsigned char)(data_020a0dea[idx * 4] - 8) < 0xf0) {
                if ((unsigned char)(data_020a0deb[idx * 4] - 0x38) < 0x20)
                    goto left;
            }
        }
        if (unk_091 != 0)
            goto right_check;
        if (!IsButtonInputValid())
            goto right_check;
    left:
        if (unk_091 == 0)
            unk_093 = (unsigned char)(data_0208ee44 << 3);
        unk_091 = 0;
        func_ov003_020b0730(this, 0);
        unk_092 = (unsigned char)(data_0208ee44 << 4);
        func_02012790(0x9a);
        unk_090 = 1;
        goto end;

    right_check:
        {
            unsigned char idx = data_020a0e40;
            if ((unsigned char)(data_020a0dea[idx * 4] - 8) < 0xf0) {
                if ((unsigned char)(data_020a0deb[idx * 4] - 0x68) < 0x20)
                    goto right;
            }
        }
        if (unk_091 != 1)
            goto end;
        if (!IsButtonInputValid())
            goto end;
    right:
        if (unk_091 == 1)
            unk_093 = (unsigned char)(data_0208ee44 << 3);
        unk_091 = 1;
        func_ov003_020b0730(this, 0);
        unk_092 = (unsigned char)(data_0208ee44 << 4);
        func_02012790(0x9b);
        unk_090 = 1;
        goto end;

state1:
        if (unk_093 != 0) {
            unk_093 -= data_0208ee44;
            if (unk_093 == 0)
                func_ov003_020b0730(this, 0);
        }
        if (unk_092 != 0) {
            unk_092 -= data_0208ee44;
            if (unk_092 == 0) {
                if (unk_091 == 0)
                    func_0202ae74();
                else
                    _ZN8dScene_c14StartSceneFadeEjjt(1, 0, 0);
                _ZN5Sound22StopLoadedMusic_Layer1Ej(0x3c);
            }
        }

end:
    return 1;
}
