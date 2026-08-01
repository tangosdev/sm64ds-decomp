#include "types.h"
typedef struct FaderBrightness {
    char pad[8];
    u32 state;  /* offset 8 */
} FaderBrightness;

extern FaderBrightness data_0209f5e8;
extern void _ZN5Scene9SetFadersEP15FaderBrightness(FaderBrightness* f);

void _ZN5Scene20SetAndStopColorFaderEv(void) {
    _ZN5Scene9SetFadersEP15FaderBrightness(&data_0209f5e8);
    data_0209f5e8.state = 0;
}
