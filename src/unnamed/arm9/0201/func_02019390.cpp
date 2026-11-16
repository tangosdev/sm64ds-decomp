//cpp
#include "OAM.h"
#include "dGraph_c.h"

extern void func_0202345c(void);
extern void func_02018efc(void);
extern "C" {

extern int func_02019018(void);
}

extern void func_0200f4b4(void);
extern "C" {

extern void func_0200f468(void);
}

extern void func_02018ec0(void);

/* The scene's graph callback (d_s_dsmt.cpp and the other scenes set it). */
extern dGraph_c::callback_c *gGraphCallback;

/* The main loop's frame start (func_020197b8, step 2). The scene's
   GraphCallback0 (gGraphCallback) returning 0 skips OAM::Reset, the
   matrix-stack reset (func_0200f4b4, while the 3D engines are on), the
   empty func_0200f468 and the stage wipe (func_02018ec0); the soft-reset
   fader (func_0202345c) and the scene fader (func_02018efc) step either
   way. */
void func_02019390(void)
{
    dGraph_c::callback_c *callback = gGraphCallback;
    if (callback != 0) {
        if (callback->GraphCallback0() == 0) {
            func_0202345c();
            func_02018efc();
            return;
        }
    }
    OAM::Reset();
    if (func_02019018() != 0) {
        func_0200f4b4();
    }
    func_0200f468();
    func_0202345c();
    func_02018efc();
    func_02018ec0();
}
