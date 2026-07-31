#include "types.h"
typedef struct FaderBrightness {
    char pad[8];
    u32 state;  /* offset 8 */
} FaderBrightness;

extern FaderBrightness DEFAULT_SCENE_FADER;
extern void _ZN5Scene9SetFadersEP15FaderBrightness(FaderBrightness* f);

void _ZN5Scene20SetAndStopColorFaderEv(void) {
    _ZN5Scene9SetFadersEP15FaderBrightness(&DEFAULT_SCENE_FADER);
    DEFAULT_SCENE_FADER.state = 0;
}
