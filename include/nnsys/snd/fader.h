#ifndef NNSYS_SND_FADER_H
#define NNSYS_SND_FADER_H

/*
 * NitroSystem's sound fader, as the Sonic Rush Adventure decomp spells it:
 * values are 8.8 fixed point (NNS_SndPlayerMoveVolume shifts its target
 * left by 8). func_02052494 has NNSi_SndFaderSet's shape: origin from the
 * current value, then target, frame and a zero counter.
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NNSSndFader {
    int origin;
    int target;
    int counter;
    int frame;
} NNSSndFader;

#ifdef __cplusplus
}
#endif

#endif
