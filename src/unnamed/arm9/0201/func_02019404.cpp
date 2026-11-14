//cpp
#include "dGraph_c.h"

/* The scene's graph callback (d_s_dsmt.cpp and the other scenes set it). */
extern dGraph_c::callback_c *gGraphCallback;

/* Run the scene's GraphCallback1: the main loop runs it after the scene step
   (func_02044120), before the frame wait. */
void func_02019404(void) {
    dGraph_c::callback_c *callback = gGraphCallback;
    if (!callback)
        return;
    callback->GraphCallback1();
}
