#include "types.h"
#include "Ov007Work.h"
#include "Ov007Settings.h"

#define func_ov007_020b7eec _Z19func_ov007_020b7eecP13Ov007BootArgs
extern void func_ov007_020b7eec(Ov007BootArgs *bootArgs);

/* Steps the sound mode by `step`, wrapping over 0..2, and redraws the
 * header. Ids 1/2/3 are the owner's sound-mode commands: its dispatcher
 * DSMT_MenuCallback maps them to SetSoundMode(2/0/1). */
void func_ov007_020b64a8(int step) {
    int idx = WORK->settings->bootArgs->soundModeItem + step;
    if (idx < 0)
        idx = 2;
    else if (idx > 2)
        idx = 0;
    switch (idx) {
    case 0:
        WORK->callback(1);
        break;
    case 1:
        WORK->callback(2);
        break;
    case 2:
        WORK->callback(3);
        break;
    }
    func_ov007_020b7eec(WORK->settings->bootArgs);
}
