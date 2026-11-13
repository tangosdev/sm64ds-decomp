#ifndef CAPTUREEFFECT_H
#define CAPTUREEFFECT_H
#ifndef __cplusplus
#define func_020497a0 _Z13func_020497a0PsS_jP13CaptureEffect
#define func_020499bc _Z13func_020499bcPsS_jP13CaptureEffect
#endif

/* The sound-capture effect at data_020a4c64. func_02049cd8 starts the
 * capture with func_02049ba8 as its callback and &data_020a4c64 as the
 * callback's argument; func_02049c48 switches the kind. On each captured
 * buffer func_02049ba8 runs the optional user callback, then the built-in
 * effect for kind: 0 none, 1 the stereo widener func_020499bc, 2 the
 * cross-echo func_020497a0. Both effects keep their state in work, which
 * is cleared (0xc0 bytes) whenever the kind changes. Plain C types: some
 * users do not include types.h. */

/* The capture callback's shape: left and right buffers, length in bytes,
 * format, then the argument. */
typedef void (*CaptureEffectCallback)(void *, void *, unsigned int, int, int);

/* One entry of the cross-echo's delay line. */
typedef struct EchoChan {
    int l; /* 0x0 */
    int r; /* 0x4 */
} EchoChan;

typedef union CaptureEffectWork {
    int history[2];    /* kind 1: the previous buffer's last two L-R differences */
    EchoChan echo[24]; /* kind 2: the 24-sample delay line */
} CaptureEffectWork;

typedef char CaptureEffectWork_size_must_be_0xc0[sizeof(CaptureEffectWork) == 0xc0 ? 1 : -1];

typedef struct CaptureEffect {
    int kind;                 /* 0x00 -- 0 none, 1 func_020499bc, 2 func_020497a0 */
    CaptureEffectCallback fn; /* 0x04 -- the user callback, run first when set */
    int arg;                  /* 0x08 -- its last argument */
    CaptureEffectWork work;   /* 0x0c */
} CaptureEffect;

#ifndef SM64DS_PLATFORM_PC
typedef char CaptureEffect_size_must_be_0xcc[sizeof(CaptureEffect) == 0xcc ? 1 : -1];
#endif

extern CaptureEffect data_020a4c64;
/* data_020a4c64.work: the ROM addresses it by its own symbol. */
extern CaptureEffectWork data_020a4c70;

extern "C" void func_02049ba8(void *, void *, unsigned int, int, CaptureEffect *);
void func_020499bc(short *, short *, unsigned int, CaptureEffect *);
void func_020497a0(short *, short *, unsigned int, CaptureEffect *);

#endif
