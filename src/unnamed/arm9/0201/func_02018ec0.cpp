//cpp
#include "dFdBrightness_c.h"



/* The scene's brightness fader (dScene_c sets it, d_scene.cpp) and the
   active transition fader: a stage wipe (dFdWipe_c, set and cleared by
   d_s_stage_methods.cpp) or the cutscene colour fader gKuppaScriptFader
   (dFdColor_c), which KuppaScript.cpp points it at for KS_WHITE/BLACK_FADE_*
   and clears on KS_LOAD_LEVEL. Both classes derive from dFdBrightness_c. */
extern dFdBrightness_c *data_0209d4ac;
extern dFdBrightness_c *data_0209d4b0;

/* Advance the scene's fader. The frame start (func_02019390) calls it every
   frame. */
void func_02018efc(void) {
    dFdBrightness_c *fader = data_0209d4ac;
    if (!fader)
        return;
    fader->AdvanceFade();
}

/* Advance the active transition fader. The frame start (func_02019390) calls
   it when the scene's GraphCallback0 lets the frame through. */
void func_02018ec0(void) {
    dFdBrightness_c *fader = data_0209d4b0;
    if (!fader)
        return;
    fader->AdvanceFade();
}
