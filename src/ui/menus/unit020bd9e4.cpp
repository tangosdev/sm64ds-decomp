//cpp
/* unit020bd9e4 -- ov007 unit 0x020bd9e4..0x020be0dc: 11 functions, .text only.
 *
 * The title menu's sound module. ov007 is the title and file-select menu
 * program that dScDSMT_c hosts; outside dScDSMT_c it has no ROM RTTI or
 * vtable, so no class name is claimed and the file keeps the unit<addr>
 * name.
 *
 * What the code does. The module owns 0x39 sound handles,
 * data_ov007_02104bbc[0..0x38], one per entry of the sound-id table
 * data_ov007_02103134, and four request flags, data_ov007_02104bb8[0..3],
 * for the looping sounds on handles 0x35..0x38. MenuSound_Create (called
 * from the init dispatcher, func_ov007_020b7138) takes the handles from the
 * "sound" heap and the flags from the "sound request" heap;
 * MenuSound_Destroy (called from func_ov007_020b6f4c, which tears every
 * menu module down) stops and releases them. func_ov007_020be098 plays one
 * sound on its own handle and func_ov007_020bdeb0 is the out-of-module
 * entry to it: a tail-call veneer the rest of the menu calls. A looping
 * sound is kept alive by func_ov007_020bde2c, which starts it if it is not
 * playing and sets its request flag; MenuSound_UpdateLoops (called from
 * func_ov007_020b7090) stops every loop that was not requested since its
 * last run and clears the flags. func_ov007_020bdbcc, 020bda8c and
 * 020bda38 keep loops 0x35, 0x36 and 0x37 alive and set their parameters
 * from their callers' values. func_ov007_020bde70 stops one sound and
 * func_ov007_020bd9e4 stops all of them, with the caller's fade argument.
 *
 * Edges: every function here touches this module's bss block
 * 0x02104ba4..0x02104bc0 or calls one that does, and nothing outside the
 * range touches it. The previous function, MenuGfx_ResetVram, is one of
 * the init dispatcher's own helpers and touches none of it; the next one,
 * func_ov007_020be0dc, belongs to the module whose bss block starts at
 * 0x02104bc0.
 *
 * Emission order: mwccarm 2004/b56 emits one .text section per function in
 * the reverse of source order, so the highest-address function is written
 * first here and the functions come out in ROM order.
 *
 */
#include "types.h"
#include "nitro/fx/fx.h"
#include "nnsys/snd/player.h"
#include "nitro/fx/fx_cp.h"
#include "math/ProgressRatio.h"

void func_0204f934(NNSSndHandle *);
void func_0204f94c(NNSSndHandle *);
int func_0204fa2c(NNSSndHandle *, int);
int func_02051f1c(void *, unsigned int, int);
extern "C" void *func_ov007_020c3df4(const char *name, int size);
extern "C" void func_ov007_020c3d1c(void *);

extern int data_ov007_02103134[];
extern char data_ov007_02103218[]; /* "sound" */
extern char data_ov007_02103220[]; /* "sound request" */
extern int data_ov007_02104ba4;
extern int data_ov007_02104ba8;
extern int data_ov007_02104bac;
extern int data_ov007_02104bb0;
extern int data_ov007_02104bb4;
extern int *data_ov007_02104bb8;
extern NNSSndHandle *data_ov007_02104bbc;

int func_ov007_020be098(int idx) {
    func_0204f94c(&data_ov007_02104bbc[idx]);
    return func_02051f1c(&data_ov007_02104bbc[idx], 2, data_ov007_02103134[idx]);
}

void MenuSound_Create(void) {
    int i;
    int j;

    data_ov007_02104bbc = (NNSSndHandle *)func_ov007_020c3df4(data_ov007_02103218, 0xe4);
    data_ov007_02104bb8 = (int *)func_ov007_020c3df4(data_ov007_02103220, 0x10);

    for (i = 0; i < 0x39; i++)
        func_0204f94c(&data_ov007_02104bbc[i]);

    for (j = 0; j < 4; j++)
        data_ov007_02104bb8[j] = 0;
}

void MenuSound_Destroy(void) {
    int i;
    for (i = 0; i < 0x39; i++) {
        if (data_ov007_02104bbc[i].player != 0) {
            func_0204fa2c(&data_ov007_02104bbc[i], 5);
        }
        func_0204f934(&data_ov007_02104bbc[i]);
    }
    if (data_ov007_02104bb8 != 0) {
        func_ov007_020c3d1c(data_ov007_02104bb8);
        data_ov007_02104bb8 = 0;
    }
    if (data_ov007_02104bbc != 0) {
        func_ov007_020c3d1c(data_ov007_02104bbc);
        data_ov007_02104bbc = 0;
    }
}

int *MenuSound_UpdateLoops(void) {
    int *arr;
    int i;
    int sound;
    for (i = 0; i < 4; i++) {
        sound = i + 0x35;
        if (data_ov007_02104bb8[i] == 0) {
            if (data_ov007_02104bbc[sound].player != 0) {
                func_0204fa2c(&data_ov007_02104bbc[sound], 1);
                /* Never true (i is a loop index, 0..3): the reset of
                 * func_ov007_020bdbcc's smoothed input looks meant for
                 * sound 0x35. The ROM keeps the test; without it 020bdebc
                 * does not match. */
                if (i == 0x35) {
                    data_ov007_02104bb4 = 0;
                    data_ov007_02104bb0 = 0;
                    data_ov007_02104bac = 0;
                }
            }
        }
        arr = data_ov007_02104bb8;
        arr[i] = 0;
    }
    return arr;
}

/* Tail-call veneer: ldr ip, [pc]; bx ip; .word func_ov007_020be098. */
int func_ov007_020bdeb0(int idx) {
    return func_ov007_020be098(idx);
}

void func_ov007_020bde70(int idx, int arg) {
    if (data_ov007_02104bbc[idx].player == 0)
        return;
    func_0204fa2c(&data_ov007_02104bbc[idx], arg);
}

void func_ov007_020bde2c(int idx) {
    if (data_ov007_02104bbc[idx].player == 0) {
        func_ov007_020be098(idx);
    }
    data_ov007_02104bb8[idx - 0x35] = 1;
}

/* Q12 lerp: from at t = 0, to at t = 0x1000. */
static inline int BlendFx(int from, int to, int t) {
    return (int)(((s64)(0x1000 - t) * from + (s64)t * to) >> 12);
}

void func_ov007_020bdbcc(int dx, int dy) {
    int x = BlendFx(data_ov007_02104bb4, dx << 12, 0x3e8);
    int y = BlendFx(data_ov007_02104bb0, dy << 12, 0x3e8);
    int lengthSq;
    int speed;
    int loudness;
    int active;
    NNSSndHandle *handle;
    int volume;
    int t;

    data_ov007_02104bb0 = y;
    lengthSq = FX_MUL(x, x) + FX_MUL(y, y);
    data_ov007_02104bb4 = x;

    speed = FX_Sqrt(lengthSq);

    if (speed <= 0x1388) {
        loudness = 0;
    } else if (speed >= 0x1b58) {
        loudness = 0x1000;
    } else {
        loudness = FX_Div(speed - 0x1388, 0x7d0);
    }

    if (data_ov007_02104bbc[0x35].player == 0) {
        data_ov007_02104ba8 = 0;
        data_ov007_02104ba4 = 0;
    }

    active = (loudness >= 0x800) ? 0x1000 : 0;

    if (active > 0) {
        data_ov007_02104ba4 = data_ov007_02104ba4 + 1;
    } else {
        data_ov007_02104ba8 = data_ov007_02104ba8 + 1;
    }

    if (data_ov007_02104ba4 > 3) {
        data_ov007_02104ba4 = 3;
        data_ov007_02104ba8 = 0;
        active = 0x1000;
    } else if (data_ov007_02104ba8 > 3) {
        active = 0;
        data_ov007_02104ba8 = 3;
        data_ov007_02104ba4 = 0;
    }

    volume = (active * 0x7f) >> 12;

    if (speed <= 0x1388) {
        t = 0;
    } else if (speed >= 0x5000) {
        t = 0x1000;
    } else {
        t = FX_Div(speed - 0x1388, 0x3c78);
    }

    handle = &data_ov007_02104bbc[0x35];
    func_ov007_020bde2c(0x35);
    NNS_SndPlayerSetVolume(handle, volume);
    NNS_SndPlayerSetTrackPitch(handle, 0xf, -(((0x1000 - t) * 0x300) >> 12));

    data_ov007_02104bac = data_ov007_02104bac + 1;
}

static inline int scale_div(int v, int range, int scale) {
    int q;
    q = ProgressRatio(v, range);
    return (q * scale) >> 12;
}

void func_ov007_020bda8c(int frames, int fadingOut) {
    NNSSndHandle *handle = &data_ov007_02104bbc[0x36];
    int depth, speed;

    if (fadingOut == 0) {
        depth = scale_div(frames, 0xb4, 0x7f);
        speed = scale_div(frames, 0x78, 0x23) + 5;
    } else {
        int left = 0x1e - frames;
        depth = scale_div(left, 0x1e, 0x7f);
        speed = scale_div(left, 0x1e, 0x23) + 5;
        if (frames == 0) {
            NNS_SndPlayerMoveVolume(handle, 0, 0x1e);
        }
    }
    func_ov007_020bde2c(0x36);
    NNS_SndPlayerSetTrackModDepth(handle, 0xf, depth);
    NNS_SndPlayerSetTrackModSpeed(handle, 0xf, speed);
}

void func_ov007_020bda38(int v) {
    NNSSndHandle *handle = &data_ov007_02104bbc[0x37];
    func_ov007_020bde2c(0x37);
    int shift = (v << 8) >> 12;
    NNS_SndPlayerSetTempoRatio(handle, shift + 0x100);
    NNS_SndPlayerSetTrackPitch(handle, 0xf, shift);
}

void func_ov007_020bd9e4(int arg) {
    int i;
    for (i = 0; i < 0x39; i++) {
        if (data_ov007_02104bbc[i].player != 0) {
            func_0204fa2c(&data_ov007_02104bbc[i], arg);
        }
    }
}
