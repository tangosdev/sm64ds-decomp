//cpp
// @symbol _ZN7dScMB_c15graphCallback_c14GraphCallback3Ev
/* dScMB_c::graphCallback_c::GraphCallback3 -- slot 3, arm9 0x02034b40.
 *
 * The Multi-Boot scene's per-frame sprite pass. It pokes whatever object
 * mOwner names, resets OAM, and -- while mEnabled -- walks the current
 * language's OamAttr list (data_0208a0f8, indexed by func_0200f0bc) drawing
 * each entry twice, once per screen.
 *
 * Two modes. While mStillTimer is zero the sprites SPIN: each entry's y comes
 * from the sine table at data_02082214, sampled at mSpinAngle stepped 0x2000
 * per entry, and mSpinAngle advances by the global frame step. Any entry whose
 * angle falls outside one full turn is drawn flat at y = 0x80 instead, and if
 * EVERY entry was flat the still frame is armed by writing 0x78 to mStillTimer.
 * While mStillTimer is nonzero the list is drawn once, unrotated and centred,
 * and the timer counts down by the same frame step.
 *
 * The OAM entry points keep their literal mangled names: OAM::Render takes
 * Fix12<int> BY VALUE, which is this tree's standing wall on turning a callee
 * into a real declaration (see the langmode audit's by-value exclusion). Only
 * this function's own symbol and fields are the compiler's here.
 */
#include "dScMB_c.h"

extern "C" {
int  func_02034d34(void *thiz);
void _ZN3OAM5ResetEv(void);
s32  func_0200f0bc(void);
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(int sub, void *attr, int x, int y, int a, int cc, int fx, int mode);
void _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(int sub, void *attr, int x, int y, int a, int cc, int fx0, int fy0, int rot, int mode);
void _ZN3OAM9RenderSubEP7OamAttrii(void *attr, int a, int b);
void func_0203083c(void);
void _ZN3OAM5FlushEv(void);
void _ZN3OAM4LoadEv(void);

extern void *data_0208a0f8[];
extern s16   data_02082214[];
extern int   data_0208ee44;
}

int dScMB_c::graphCallback_c::GraphCallback3()
{
    if (mOwner != 0) {
        func_02034d34(mOwner);
    }
    _ZN3OAM5ResetEv();

    if (mEnabled != 0) {
        int id = func_0200f0bc();
        void *attr = data_0208a0f8[id];

        if (mStillTimer == 0) {
            int sb = mSpinAngle;
            int x = 0x80;
            int one = 1;
            int arr[3];
            arr[0] = 0;
            arr[1] = 0;
            arr[2] = 0;

            do {
                int y;
                if (sb >= 0 && sb < 0x10000) {
                    u16 t = (u16)sb;
                    s16 ang = (s16)t;
                    u16 t2 = (u16)ang;
                    y = data_02082214[(t2 >> 4) * 2];
                    y = (y >> 10) + 0x80;
                    arr[0] = one;
                } else {
                    y = x;
                }

                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(arr[1], attr, x, y, -1, -1, 0x1000, arr[1]);
                _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiEi(one, attr, x, y, -1, -1, 0x1000, arr[2]);

                if (*(u16 *)((char *)attr + 6) == 0xffff)
                    break;
                attr = (char *)attr + 8;
                sb -= 0x2000;
            } while (1);

            if (mAdvancing != 0) {
                mSpinAngle += (data_0208ee44 << 10);
                if (arr[0] == 0)
                    mStillTimer = 0x78;
            }
        } else {
            _ZN3OAM6RenderEbP7OamAttriiii5Fix12IiES3_ii(0, attr, 0x80, 0x80, -1, -1, 0x1000, 0x1000, 0, -1);
            _ZN3OAM9RenderSubEP7OamAttrii(attr, 0x80, 0x80);
            mStillTimer -= data_0208ee44;
            mSpinAngle = 0;
        }
    }

    func_0203083c();
    _ZN3OAM5FlushEv();
    _ZN3OAM4LoadEv();
    return 1;
}
