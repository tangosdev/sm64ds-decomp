/* Zero gGlobalTimer and data_020a0db4 at boot (func_0201a054). The text
   boxes (Message2.cpp, Message3.cpp), dScStage_c::RenderBouncingArrows and
   the Monte Carlo cards blink on gGlobalTimer's low bits, and the star
   select spins a model by it, so it reads as a running frame count. */
extern int gGlobalTimer, data_020a0db4;
void func_0203b684(void) {
    gGlobalTimer = 0;
    data_020a0db4 = 0;
}
