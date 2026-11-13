//cpp
#include "types.h"
#include "nnsys/snd/player.h"

extern s32 data_020a4c5c[];
extern s32 data_020a4c4c[];
extern u16 data_020a4c48[];
extern s32 data_020a4c54[];

/* Step the two track-mask volume ramps func_020494cc set up: move each
   current volume (data_020a4c5c) by its step (data_020a4c4c) toward its
   target (data_020a4c54), stopping the ramp there, and send it through
   NNS_SndPlayerSetTrackVolume in 0..127. The tracks are muted with
   NNS_SndPlayerSetTrackMute when a falling ramp drops below 0 and unmuted
   when a rising one leaves 0. func_020132d8 calls it every frame on
   sMusicHandle. */
void func_020490b0(NNSSndHandle *c) {
    s32 t[4];
    s32 i;
    s32 a;
    s32 e;
    s32 w;
    s32 flag;

    if (c == 0) {
        return;
    }
    if (c->player == 0) {
        return;
    }

    t[0] = 0;
    t[1] = 0x7f000;
    t[2] = 0;
    t[3] = 0;

    for (i = 0; i < 2; i++) {
        flag = 0;
        a = data_020a4c4c[i];
        if (a > 0) {
            e = data_020a4c5c[i];
            if (e == 0) {
                flag = 1;
            }
            data_020a4c5c[i] = data_020a4c5c[i] + a;
            if (data_020a4c5c[i] >= data_020a4c54[i]) {
                data_020a4c5c[i] = data_020a4c54[i];
                data_020a4c4c[i] = t[0];
            }
        } else if (a < 0) {
            data_020a4c5c[i] = data_020a4c5c[i] + a;
            if (data_020a4c5c[i] <= data_020a4c54[i]) {
                data_020a4c5c[i] = data_020a4c54[i];
                data_020a4c4c[i] = 0;
            }
        } else {
            continue;
        }

        w = data_020a4c5c[i];
        if (w > 0x7f000) {
            data_020a4c5c[i] = t[1];
        } else if (w < 0) {
            data_020a4c5c[i] = t[2];
            NNS_SndPlayerSetTrackMute(c, data_020a4c48[i], 1);
        }
        if (flag != 0) {
            NNS_SndPlayerSetTrackMute(c, data_020a4c48[i], t[3]);
        }
        NNS_SndPlayerSetTrackVolume(c, data_020a4c48[i], data_020a4c5c[i] >> 12);
    }
}
