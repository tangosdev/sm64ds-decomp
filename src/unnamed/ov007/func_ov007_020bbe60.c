#include "types.h"
#include "Ov007Work.h"
#include "nitro/fx/fx.h"
#include "Ov007Picker.h"
#include "private/ov007_picker_scene.h"

/* Six portrait drawings, selected by a two-way selector and three-step cycle.
 * Plotting the byte coordinates produces Mario, Luigi and Yoshi portraits.
 * Names are reconstructed from that evidence, not recovered identifiers.
 * The projector uses these as screen X/Y inputs and negates Y; depth is separate.
 * The nested stroke arrays remain ROM imports; threshold units are unknown.
 */
static const int sPortraitStrokeCounts[6] = {30, 34, 36, 20, 32, 32};
extern const int data_ov007_020cf700[30];
extern const int data_ov007_020cfa70[34];
extern const int data_ov007_020cfb84[37];
extern const int data_ov007_020cef80[20];
extern const int data_ov007_020cf96c[32];
extern const int data_ov007_020cf8ec[32];
static const int *sPortraitStrokePointCounts[6] = {data_ov007_020cf700, data_ov007_020cfa70,
                                                   data_ov007_020cfb84, data_ov007_020cef80,
                                                   data_ov007_020cf96c, data_ov007_020cf8ec};
extern const unsigned char *data_ov007_020d7ffc[30];
extern const unsigned char *data_ov007_020d8464[34];
extern const unsigned char *data_ov007_020d85fc[37];
extern const unsigned char *data_ov007_020d7f0c[20];
extern const unsigned char *data_ov007_020d82e4[32];
extern const unsigned char *data_ov007_020d8164[32];
static const unsigned char *const *sPortraitStrokeXCoords[6] = {
    data_ov007_020d7ffc, data_ov007_020d8464, data_ov007_020d85fc,
    data_ov007_020d7f0c, data_ov007_020d82e4, data_ov007_020d8164};
extern const unsigned char *data_ov007_020d8074[30];
extern const unsigned char *data_ov007_020d84ec[34];
extern const unsigned char *data_ov007_020d8690[37];
extern const unsigned char *data_ov007_020d7f5c[20];
extern const unsigned char *data_ov007_020d8364[32];
extern const unsigned char *data_ov007_020d81e4[32];
static const unsigned char *const *sPortraitStrokeYCoords[6] = {
    data_ov007_020d8074, data_ov007_020d84ec, data_ov007_020d8690,
    data_ov007_020d7f5c, data_ov007_020d8364, data_ov007_020d81e4};
extern const int *data_ov007_020d80ec[30];
extern const int *data_ov007_020d8574[34];
extern const int *data_ov007_020d8724[37];
extern const int *data_ov007_020d7fac[20];
extern const int *data_ov007_020d83e4[32];
extern const int *data_ov007_020d8264[32];
static const int *const *sPortraitStrokeProgressThresholds[6] = {
    data_ov007_020d80ec, data_ov007_020d8574, data_ov007_020d8724,
    data_ov007_020d7fac, data_ov007_020d83e4, data_ov007_020d8264};

extern char *data_ov007_02104ba0;

#define SCENE ((Ov007PickerScene *)data_ov007_02104ba0)

/* The per-figure stroke stages as a pointer: indexing through it, not
 * through WORK->strokeStage, gives the ROM's base + 0xfc then [idx] form. */
static inline u8 *StrokeStages(void) {
    return WORK->strokeStage;
}

/* Loads figure idx's next portrait drawing (strokeStage[idx]: 0, 1, 2, then
 * back to 0) into the picker, with a replay of 120 frames; figure 0's third
 * drawing gets 1.5 times that. */
void func_ov007_020bbe60(int idx) {
    int drawing = idx * 3 + StrokeStages()[idx];
    func_ov007_020c6784(SCENE->picker, sPortraitStrokeCounts[drawing],
                        sPortraitStrokePointCounts[drawing], sPortraitStrokeXCoords[drawing],
                        sPortraitStrokeYCoords[drawing], sPortraitStrokeProgressThresholds[drawing],
                        1);
    SCENE->replayFrames = 120;
    if (idx == 0 && StrokeStages()[idx] == 2) {
        int frames = FX_MUL(SCENE->replayFrames << 12, 0x1800);
        SCENE->replayFrames = frames >> 12;
    }
    StrokeStages()[idx]++;
    if (StrokeStages()[idx] >= 3)
        StrokeStages()[idx] = 0;
}

const int data_ov007_020cef80[20] = {
    27, 14, 5, 8, 4, 3, 19, 10, 63, 11, 9, 31, 15, 11, 22, 26, 5, 20, 5, 4,
};
const int data_ov007_020cf700[30] = {
    16, 12, 10, 17, 7, 8,  52, 19, 16, 10, 13, 12, 24, 12, 13,
    3,  2,  12, 13, 3, 77, 17, 20, 6,  8,  2,  2,  7,  7,  3,
};
const int data_ov007_020cf8ec[32] = {
    27, 14, 5, 8, 4,  3, 19, 10, 63, 11, 9, 31, 15, 11, 22, 26,
    5,  20, 5, 4, 37, 5, 5,  8,  9,  7,  5, 10, 8,  7,  5,  7,
};
const int data_ov007_020cf96c[32] = {
    27, 14, 5, 8, 4,  3,  19, 10, 63, 11, 9, 31, 15, 11, 22, 26,
    5,  20, 5, 4, 28, 42, 26, 3,  1,  4,  6, 6,  5,  5,  4,  3,
};
const int data_ov007_020cfa70[34] = {
    22, 9,  5, 5,  2,  15, 11, 4,  1,  8,  6,  25, 25, 3,  21, 12, 5,
    23, 10, 4, 14, 24, 30, 38, 20, 12, 13, 11, 33, 31, 18, 1,  1,  2,
};
const int data_ov007_020cfb84[37] = {
    18, 7,  5,  14, 6, 3, 10, 7, 8,  20, 26, 20, 22, 6, 3, 14, 8, 20, 31,
    3,  26, 14, 19, 9, 7, 35, 9, 17, 19, 5,  6,  9,  7, 6, 14, 8, 1,
};
extern const unsigned char data_ov007_020ccca4[];
extern const unsigned char data_ov007_020ccca8[];
extern const unsigned char data_ov007_020cccac[];
extern const unsigned char data_ov007_020cccb0[];
extern const unsigned char data_ov007_020cccb4[];
extern const unsigned char data_ov007_020cccb8[];
extern const unsigned char data_ov007_020cccbc[];
extern const unsigned char data_ov007_020cccc0[];
extern const unsigned char data_ov007_020cccc4[];
extern const unsigned char data_ov007_020cccc8[];
extern const unsigned char data_ov007_020ccccc[];
extern const unsigned char data_ov007_020cccd0[];
extern const unsigned char data_ov007_020cccdc[];
extern const unsigned char data_ov007_020ccce0[];
extern const unsigned char data_ov007_020ccce4[];
extern const unsigned char data_ov007_020cccec[];
extern const unsigned char data_ov007_020cccf4[];
extern const unsigned char data_ov007_020cccfc[];
extern const unsigned char data_ov007_020ccd00[];
extern const unsigned char data_ov007_020ccd04[];
extern const unsigned char data_ov007_020ccd08[];
extern const unsigned char data_ov007_020ccd0c[];
extern const unsigned char data_ov007_020ccd10[];
extern const unsigned char data_ov007_020ccd14[];
extern const unsigned char data_ov007_020ccd18[];
extern const unsigned char data_ov007_020ccd1c[];
extern const unsigned char data_ov007_020ccd20[];
extern const unsigned char data_ov007_020ccd24[];
extern const unsigned char data_ov007_020ccd28[];
extern const unsigned char data_ov007_020ccd2c[];
extern const unsigned char data_ov007_020ccd30[];
extern const unsigned char data_ov007_020ccd34[];
extern const unsigned char data_ov007_020ccd38[];
extern const unsigned char data_ov007_020ccd3c[];
extern const unsigned char data_ov007_020ccd40[];
extern const unsigned char data_ov007_020ccd44[];
extern const unsigned char data_ov007_020ccd48[];
extern const unsigned char data_ov007_020ccd4c[];
extern const unsigned char data_ov007_020ccd50[];
extern const unsigned char data_ov007_020ccd54[];
extern const int data_ov007_020ccd64[];
extern const unsigned char data_ov007_020ccd6c[];
extern const int data_ov007_020ccd70[];
extern const unsigned char data_ov007_020ccd74[];
extern const unsigned char data_ov007_020ccd78[];
extern const unsigned char data_ov007_020ccd80[];
extern const unsigned char data_ov007_020ccd84[];
extern const unsigned char data_ov007_020ccd88[];
extern const unsigned char data_ov007_020ccd8c[];
extern const unsigned char data_ov007_020ccd94[];
extern const int data_ov007_020ccd98[];
extern const unsigned char data_ov007_020ccd9c[];
extern const int data_ov007_020ccda0[];
extern const int data_ov007_020ccda4[];
extern const unsigned char data_ov007_020ccda8[];
extern const unsigned char data_ov007_020ccdb0[];
extern const unsigned char data_ov007_020ccdb4[];
extern const unsigned char data_ov007_020ccdbc[];
extern const unsigned char data_ov007_020ccdc4[];
extern const unsigned char data_ov007_020ccdcc[];
extern const unsigned char data_ov007_020ccdd4[];
extern const unsigned char data_ov007_020ccddc[];
extern const unsigned char data_ov007_020ccde4[];
extern const unsigned char data_ov007_020ccdec[];
extern const unsigned char data_ov007_020ccdf4[];
extern const unsigned char data_ov007_020ccdfc[];
extern const unsigned char data_ov007_020cce04[];
extern const unsigned char data_ov007_020cce0c[];
extern const unsigned char data_ov007_020cce14[];
extern const unsigned char data_ov007_020cce1c[];
extern const unsigned char data_ov007_020cce24[];
extern const unsigned char data_ov007_020cce2c[];
extern const unsigned char data_ov007_020cce34[];
extern const unsigned char data_ov007_020cce3c[];
extern const unsigned char data_ov007_020cce44[];
extern const unsigned char data_ov007_020cce4c[];
extern const unsigned char data_ov007_020cce54[];
extern const unsigned char data_ov007_020cce5c[];
extern const unsigned char data_ov007_020cce64[];
extern const unsigned char data_ov007_020cce6c[];
extern const unsigned char data_ov007_020cce74[];
extern const unsigned char data_ov007_020cce7c[];
extern const unsigned char data_ov007_020cce84[];
extern const unsigned char data_ov007_020cce8c[];
extern const unsigned char data_ov007_020cce94[];
extern const unsigned char data_ov007_020cce9c[];
extern const unsigned char data_ov007_020ccea4[];
extern const unsigned char data_ov007_020cceac[];
extern const unsigned char data_ov007_020cceb4[];
extern const unsigned char data_ov007_020ccebc[];
extern const unsigned char data_ov007_020ccec4[];
extern const unsigned char data_ov007_020ccecc[];
extern const unsigned char data_ov007_020cced4[];
extern const unsigned char data_ov007_020ccedc[];
extern const unsigned char data_ov007_020ccee4[];
extern const unsigned char data_ov007_020cceec[];
extern const unsigned char data_ov007_020ccef4[];
extern const unsigned char data_ov007_020ccefc[];
extern const unsigned char data_ov007_020ccf04[];
extern const unsigned char data_ov007_020ccf0c[];
extern const unsigned char data_ov007_020ccf14[];
extern const unsigned char data_ov007_020ccf1c[];
extern const unsigned char data_ov007_020ccf24[];
extern const unsigned char data_ov007_020ccf2c[];
extern const unsigned char data_ov007_020ccf34[];
extern const unsigned char data_ov007_020ccf3c[];
extern const unsigned char data_ov007_020ccf44[];
extern const unsigned char data_ov007_020ccf4c[];
extern const unsigned char data_ov007_020ccf54[];
extern const unsigned char data_ov007_020ccf5c[];
extern const unsigned char data_ov007_020ccf64[];
extern const unsigned char data_ov007_020ccf6c[];
extern const unsigned char data_ov007_020ccf74[];
extern const unsigned char data_ov007_020ccf7c[];
extern const unsigned char data_ov007_020ccf84[];
extern const unsigned char data_ov007_020ccf8c[];
extern const unsigned char data_ov007_020ccf94[];
extern const unsigned char data_ov007_020ccf9c[];
extern const unsigned char data_ov007_020ccfa4[];
extern const unsigned char data_ov007_020ccfac[];
extern const unsigned char data_ov007_020ccfb4[];
extern const unsigned char data_ov007_020ccfbc[];
extern const unsigned char data_ov007_020ccfc4[];
extern const unsigned char data_ov007_020ccfcc[];
extern const unsigned char data_ov007_020ccfd4[];
extern const int data_ov007_020ccfdc[];
extern const unsigned char data_ov007_020ccfe4[];
extern const int data_ov007_020ccfec[];
extern const unsigned char data_ov007_020ccff4[];
extern const int data_ov007_020ccffc[];
extern const int data_ov007_020cd004[];
extern const unsigned char data_ov007_020cd00c[];
extern const unsigned char data_ov007_020cd014[];
extern const unsigned char data_ov007_020cd01c[];
extern const unsigned char data_ov007_020cd024[];
extern const unsigned char data_ov007_020cd02c[];
extern const unsigned char data_ov007_020cd034[];
extern const unsigned char data_ov007_020cd044[];
extern const unsigned char data_ov007_020cd04c[];
extern const unsigned char data_ov007_020cd054[];
extern const unsigned char data_ov007_020cd05c[];
extern const int data_ov007_020cd064[];
extern const unsigned char data_ov007_020cd06c[];
extern const unsigned char data_ov007_020cd074[];
extern const unsigned char data_ov007_020cd07c[];
extern const unsigned char data_ov007_020cd088[];
extern const unsigned char data_ov007_020cd094[];
extern const unsigned char data_ov007_020cd0a0[];
extern const unsigned char data_ov007_020cd0ac[];
extern const unsigned char data_ov007_020cd0b8[];
extern const unsigned char data_ov007_020cd0c4[];
extern const unsigned char data_ov007_020cd0d0[];
extern const unsigned char data_ov007_020cd0dc[];
extern const unsigned char data_ov007_020cd0e8[];
extern const unsigned char data_ov007_020cd0f4[];
extern const unsigned char data_ov007_020cd100[];
extern const unsigned char data_ov007_020cd10c[];
extern const unsigned char data_ov007_020cd118[];
extern const unsigned char data_ov007_020cd124[];
extern const unsigned char data_ov007_020cd130[];
extern const unsigned char data_ov007_020cd13c[];
extern const unsigned char data_ov007_020cd148[];
extern const unsigned char data_ov007_020cd154[];
extern const unsigned char data_ov007_020cd160[];
extern const unsigned char data_ov007_020cd16c[];
extern const unsigned char data_ov007_020cd178[];
extern const unsigned char data_ov007_020cd184[];
extern const unsigned char data_ov007_020cd190[];
extern const unsigned char data_ov007_020cd19c[];
extern const unsigned char data_ov007_020cd1a8[];
extern const unsigned char data_ov007_020cd1b4[];
extern const unsigned char data_ov007_020cd1c0[];
extern const unsigned char data_ov007_020cd1cc[];
extern const unsigned char data_ov007_020cd1d8[];
extern const unsigned char data_ov007_020cd1e4[];
extern const unsigned char data_ov007_020cd1f0[];
extern const unsigned char data_ov007_020cd1fc[];
extern const unsigned char data_ov007_020cd208[];
extern const int data_ov007_020cd214[];
extern const int data_ov007_020cd220[];
extern const int data_ov007_020cd22c[];
extern const int data_ov007_020cd238[];
extern const int data_ov007_020cd244[];
extern const int data_ov007_020cd250[];
extern const int data_ov007_020cd25c[];
extern const unsigned char data_ov007_020cd268[];
extern const unsigned char data_ov007_020cd274[];
extern const int data_ov007_020cd280[];
extern const unsigned char data_ov007_020cd28c[];
extern const unsigned char data_ov007_020cd298[];
extern const int data_ov007_020cd2a4[];
extern const unsigned char data_ov007_020cd2b0[];
extern const unsigned char data_ov007_020cd2bc[];
extern const unsigned char data_ov007_020cd2c8[];
extern const unsigned char data_ov007_020cd2d4[];
extern const int data_ov007_020cd2e0[];
extern const unsigned char data_ov007_020cd2ec[];
extern const unsigned char data_ov007_020cd2f8[];
extern const unsigned char data_ov007_020cd304[];
extern const unsigned char data_ov007_020cd314[];
extern const unsigned char data_ov007_020cd324[];
extern const unsigned char data_ov007_020cd334[];
extern const unsigned char data_ov007_020cd344[];
extern const unsigned char data_ov007_020cd354[];
extern const unsigned char data_ov007_020cd364[];
extern const unsigned char data_ov007_020cd374[];
extern const unsigned char data_ov007_020cd384[];
extern const unsigned char data_ov007_020cd394[];
extern const unsigned char data_ov007_020cd3a4[];
extern const unsigned char data_ov007_020cd3b4[];
extern const unsigned char data_ov007_020cd3c4[];
extern const unsigned char data_ov007_020cd3d4[];
extern const unsigned char data_ov007_020cd3e4[];
extern const unsigned char data_ov007_020cd3f4[];
extern const unsigned char data_ov007_020cd404[];
extern const unsigned char data_ov007_020cd414[];
extern const unsigned char data_ov007_020cd424[];
extern const unsigned char data_ov007_020cd434[];
extern const unsigned char data_ov007_020cd444[];
extern const unsigned char data_ov007_020cd454[];
extern const unsigned char data_ov007_020cd464[];
extern const unsigned char data_ov007_020cd474[];
extern const int data_ov007_020cd484[];
extern const int data_ov007_020cd494[];
extern const int data_ov007_020cd4a4[];
extern const int data_ov007_020cd4b4[];
extern const int data_ov007_020cd4c4[];
extern const unsigned char data_ov007_020cd4d4[];
extern const unsigned char data_ov007_020cd4e4[];
extern const unsigned char data_ov007_020cd4f4[];
extern const unsigned char data_ov007_020cd504[];
extern const int data_ov007_020cd514[];
extern const unsigned char data_ov007_020cd524[];
extern const unsigned char data_ov007_020cd538[];
extern const unsigned char data_ov007_020cd54c[];
extern const unsigned char data_ov007_020cd560[];
extern const unsigned char data_ov007_020cd574[];
extern const unsigned char data_ov007_020cd588[];
extern const unsigned char data_ov007_020cd59c[];
extern const unsigned char data_ov007_020cd5b0[];
extern const unsigned char data_ov007_020cd5c4[];
extern const unsigned char data_ov007_020cd5d8[];
extern const unsigned char data_ov007_020cd5ec[];
extern const unsigned char data_ov007_020cd600[];
extern const unsigned char data_ov007_020cd614[];
extern const unsigned char data_ov007_020cd628[];
extern const unsigned char data_ov007_020cd63c[];
extern const unsigned char data_ov007_020cd650[];
extern const unsigned char data_ov007_020cd664[];
extern const unsigned char data_ov007_020cd678[];
extern const unsigned char data_ov007_020cd68c[];
extern const int data_ov007_020cd6a0[];
extern const int data_ov007_020cd6b4[];
extern const unsigned char data_ov007_020cd6c8[];
extern const unsigned char data_ov007_020cd6dc[];
extern const int data_ov007_020cd6f0[];
extern const int data_ov007_020cd704[];
extern const unsigned char data_ov007_020cd718[];
extern const unsigned char data_ov007_020cd72c[];
extern const unsigned char data_ov007_020cd740[];
extern const unsigned char data_ov007_020cd754[];
extern const unsigned char data_ov007_020cd768[];
extern const int data_ov007_020cd77c[];
extern const int data_ov007_020cd790[];
extern const int data_ov007_020cd7a4[];
extern const unsigned char data_ov007_020cd7b8[];
extern const unsigned char data_ov007_020cd7cc[];
extern const int data_ov007_020cd7e0[];
extern const int data_ov007_020cd7f4[];
extern const int data_ov007_020cd808[];
extern const int data_ov007_020cd81c[];
extern const unsigned char data_ov007_020cd830[];
extern const int data_ov007_020cd844[];
extern const int data_ov007_020cd858[];
extern const int data_ov007_020cd86c[];
extern const unsigned char data_ov007_020cd880[];
extern const unsigned char data_ov007_020cd894[];
extern const unsigned char data_ov007_020cd8ac[];
extern const unsigned char data_ov007_020cd8c4[];
extern const unsigned char data_ov007_020cd8dc[];
extern const unsigned char data_ov007_020cd8f4[];
extern const unsigned char data_ov007_020cd90c[];
extern const unsigned char data_ov007_020cd924[];
extern const unsigned char data_ov007_020cd93c[];
extern const unsigned char data_ov007_020cd954[];
extern const unsigned char data_ov007_020cd96c[];
extern const unsigned char data_ov007_020cd984[];
extern const int data_ov007_020cd99c[];
extern const unsigned char data_ov007_020cd9cc[];
extern const int data_ov007_020cd9fc[];
extern const int data_ov007_020cda44[];
extern const int data_ov007_020cda74[];
extern const int data_ov007_020cda8c[];
extern const int data_ov007_020cdaa4[];
extern const int data_ov007_020cdad4[];
extern const int data_ov007_020cdaec[];
extern const unsigned char data_ov007_020cdb4c[];
extern const unsigned char data_ov007_020cdb64[];
extern const unsigned char data_ov007_020cdb7c[];
extern const unsigned char data_ov007_020cdb98[];
extern const unsigned char data_ov007_020cdbb4[];
extern const unsigned char data_ov007_020cdbd0[];
extern const unsigned char data_ov007_020cdbec[];
extern const unsigned char data_ov007_020cdc08[];
extern const unsigned char data_ov007_020cdc24[];
extern const unsigned char data_ov007_020cdc40[];
extern const unsigned char data_ov007_020cdc5c[];
extern const unsigned char data_ov007_020cdc78[];
extern const unsigned char data_ov007_020cdc94[];
extern const unsigned char data_ov007_020cdcb0[];
extern const unsigned char data_ov007_020cdccc[];
extern const unsigned char data_ov007_020cdce8[];
extern const int data_ov007_020cdd04[];
extern const int data_ov007_020cdd20[];
extern const int data_ov007_020cdd3c[];
extern const int data_ov007_020cdd58[];
extern const unsigned char data_ov007_020cdd74[];
extern const unsigned char data_ov007_020cdd90[];
extern const int data_ov007_020cddac[];
extern const int data_ov007_020cddc8[];
extern const int data_ov007_020cdde4[];
extern const int data_ov007_020cde00[];
extern const int data_ov007_020cde1c[];
extern const int data_ov007_020cde38[];
extern const unsigned char data_ov007_020cde54[];
extern const unsigned char data_ov007_020cde74[];
extern const unsigned char data_ov007_020cde94[];
extern const unsigned char data_ov007_020cdeb4[];
extern const unsigned char data_ov007_020cded4[];
extern const unsigned char data_ov007_020cdef4[];
extern const unsigned char data_ov007_020cdf14[];
extern const unsigned char data_ov007_020cdf34[];
extern const int data_ov007_020cdf54[];
extern const int data_ov007_020cdf74[];
extern const int data_ov007_020cdf94[];
extern const int data_ov007_020cdfb4[];
extern const int data_ov007_020cdfd4[];
extern const int data_ov007_020cdff4[];
extern const int data_ov007_020ce014[];
extern const int data_ov007_020ce034[];
extern const int data_ov007_020ce054[];
extern const unsigned char data_ov007_020ce074[];
extern const unsigned char data_ov007_020ce098[];
extern const unsigned char data_ov007_020ce0bc[];
extern const unsigned char data_ov007_020ce0e0[];
extern const int data_ov007_020ce104[];
extern const int data_ov007_020ce128[];
extern const int data_ov007_020ce14c[];
extern const int data_ov007_020ce170[];
extern const int data_ov007_020ce194[];
extern const int data_ov007_020ce1b8[];
extern const unsigned char data_ov007_020ce1dc[];
extern const unsigned char data_ov007_020ce204[];
extern const unsigned char data_ov007_020ce22c[];
extern const unsigned char data_ov007_020ce254[];
extern const int data_ov007_020ce27c[];
extern const int data_ov007_020ce2a4[];
extern const int data_ov007_020ce2cc[];
extern const int data_ov007_020ce2f4[];
extern const int data_ov007_020ce31c[];
extern const int data_ov007_020ce344[];
extern const unsigned char data_ov007_020ce36c[];
extern const unsigned char data_ov007_020ce398[];
extern const int data_ov007_020ce3c4[];
extern const int data_ov007_020ce3f0[];
extern const int data_ov007_020ce41c[];
extern const int data_ov007_020ce448[];
extern const int data_ov007_020ce474[];
extern const int data_ov007_020ce4a4[];
extern const int data_ov007_020ce4d4[];
extern const int data_ov007_020ce504[];
extern const int data_ov007_020ce534[];
extern const int data_ov007_020ce564[];
extern const int data_ov007_020ce594[];
extern const unsigned char data_ov007_020ce5c8[];
extern const unsigned char data_ov007_020ce5fc[];
extern const int data_ov007_020ce630[];
extern const int data_ov007_020ce664[];
extern const int data_ov007_020ce698[];
extern const int data_ov007_020ce704[];
extern const int data_ov007_020ce774[];
extern const int data_ov007_020ce7ac[];
extern const int data_ov007_020ce7e4[];
extern const int data_ov007_020ce81c[];
extern const int data_ov007_020ce88c[];
extern const int data_ov007_020ce8fc[];
extern const int data_ov007_020ce938[];
extern const unsigned char data_ov007_020ce974[];
extern const unsigned char data_ov007_020ce9b4[];
extern const int data_ov007_020ce9f4[];
extern const int data_ov007_020cea34[];
extern const int data_ov007_020cea74[];
extern const int data_ov007_020ceab8[];
extern const int data_ov007_020ceafc[];
extern const int data_ov007_020ceb40[];
extern const int data_ov007_020ceb88[];
extern const int data_ov007_020cebd0[];
extern const int data_ov007_020cec1c[];
extern const int data_ov007_020cec68[];
extern const int data_ov007_020cecb4[];
extern const unsigned char data_ov007_020ced00[];
extern const unsigned char data_ov007_020ced50[];
extern const int data_ov007_020ceda0[];
extern const int data_ov007_020cedf0[];
extern const int data_ov007_020cee40[];
extern const int data_ov007_020cee90[];
extern const int data_ov007_020ceee0[];
extern const int data_ov007_020cef30[];
extern const int data_ov007_020cefd0[];
extern const int data_ov007_020cf120[];
extern const int data_ov007_020cf178[];
extern const int data_ov007_020cf1d0[];
extern const int data_ov007_020cf228[];
extern const int data_ov007_020cf284[];
extern const int data_ov007_020cf2e4[];
extern const int data_ov007_020cf344[];
extern const int data_ov007_020cf3a8[];
extern const int data_ov007_020cf40c[];
extern const int data_ov007_020cf474[];
extern const int data_ov007_020cf4dc[];
extern const int data_ov007_020cf544[];
extern const int data_ov007_020cf5ac[];
extern const int data_ov007_020cf618[];
extern const int data_ov007_020cf688[];
extern const int data_ov007_020cf778[];
extern const int data_ov007_020cf7f4[];
extern const int data_ov007_020cf870[];
extern const int data_ov007_020cf9ec[];
extern const int data_ov007_020cfaf8[];
extern const int data_ov007_020cfc18[];
extern const int data_ov007_020cfcac[];
extern const int data_ov007_020cfd44[];
extern const int data_ov007_020cfe94[];
extern const int data_ov007_020cff64[];
extern const int data_ov007_020d0060[];

const unsigned char *data_ov007_020d7f0c[20] = {
    data_ov007_020cdccc, data_ov007_020cd394, data_ov007_020cce7c, data_ov007_020cd074,
    data_ov007_020ccd74, data_ov007_020ccd10, data_ov007_020cd5ec, data_ov007_020cd13c,
    data_ov007_020ce974, data_ov007_020cd1cc, data_ov007_020cd0ac, data_ov007_020cdf14,
    data_ov007_020cd464, data_ov007_020cd1e4, data_ov007_020cd8f4, data_ov007_020cdc94,
    data_ov007_020cce64, data_ov007_020cd7b8, data_ov007_020cce24, data_ov007_020ccd78,
};
const unsigned char *data_ov007_020d7f5c[20] = {
    data_ov007_020cdce8, data_ov007_020cd3a4, data_ov007_020ccddc, data_ov007_020cd06c,
    data_ov007_020ccdb4, data_ov007_020ccd48, data_ov007_020cd600, data_ov007_020cd148,
    data_ov007_020ce9b4, data_ov007_020cd1d8, data_ov007_020cd0b8, data_ov007_020cdf34,
    data_ov007_020cd454, data_ov007_020cd1f0, data_ov007_020cd90c, data_ov007_020cdcb0,
    data_ov007_020ccdc4, data_ov007_020cd7cc, data_ov007_020cce4c, data_ov007_020ccd6c,
};
const int *data_ov007_020d7fac[20] = {
    data_ov007_020cf5ac, data_ov007_020ce774, data_ov007_020cd704, data_ov007_020cdfb4,
    data_ov007_020cd484, data_ov007_020cd214, data_ov007_020cebd0, data_ov007_020ce2cc,
    data_ov007_020cff64, data_ov007_020ce3f0, data_ov007_020ce104, data_ov007_020cf870,
    data_ov007_020ce8fc, data_ov007_020ce41c, data_ov007_020cf178, data_ov007_020cf544,
    data_ov007_020cd7a4, data_ov007_020cef30, data_ov007_020cd7f4, data_ov007_020cd4b4,
};
const unsigned char *data_ov007_020d7ffc[30] = {
    data_ov007_020cd4d4, data_ov007_020cd268, data_ov007_020cd154, data_ov007_020cd574,
    data_ov007_020ccf94, data_ov007_020cd04c, data_ov007_020ce5c8, data_ov007_020cd664,
    data_ov007_020cd4f4, data_ov007_020cd178, data_ov007_020cd334, data_ov007_020cd2b0,
    data_ov007_020cdb4c, data_ov007_020cd2c8, data_ov007_020cd354, data_ov007_020ccd54,
    data_ov007_020ccd00, data_ov007_020cd2ec, data_ov007_020cd374, data_ov007_020ccd08,
    data_ov007_020ced00, data_ov007_020cd538, data_ov007_020cd880, data_ov007_020cce9c,
    data_ov007_020ccfcc, data_ov007_020ccccc, data_ov007_020cccdc, data_ov007_020ccf64,
    data_ov007_020ccf34, data_ov007_020ccd1c,
};
const unsigned char *data_ov007_020d8074[30] = {
    data_ov007_020cd4e4, data_ov007_020cd274, data_ov007_020cd160, data_ov007_020cd588,
    data_ov007_020ccf8c, data_ov007_020cd054, data_ov007_020ce5fc, data_ov007_020cd678,
    data_ov007_020cd504, data_ov007_020cd184, data_ov007_020cd344, data_ov007_020cd2bc,
    data_ov007_020cdb64, data_ov007_020cd2d4, data_ov007_020cd364, data_ov007_020ccd0c,
    data_ov007_020ccce0, data_ov007_020cd2f8, data_ov007_020cd314, data_ov007_020ccd3c,
    data_ov007_020ced50, data_ov007_020cd524, data_ov007_020cd68c, data_ov007_020cceb4,
    data_ov007_020ccfe4, data_ov007_020cccf4, data_ov007_020cccd0, data_ov007_020ccf5c,
    data_ov007_020ccfb4, data_ov007_020ccd44,
};
const int *data_ov007_020d80ec[30] = {
    data_ov007_020ce9f4, data_ov007_020ce4d4, data_ov007_020ce2f4, data_ov007_020ceafc,
    data_ov007_020cde1c, data_ov007_020ce034, data_ov007_020cfe94, data_ov007_020cecb4,
    data_ov007_020cea34, data_ov007_020ce31c, data_ov007_020ce630, data_ov007_020ce504,
    data_ov007_020cf2e4, data_ov007_020ce534, data_ov007_020ce664, data_ov007_020cd2e0,
    data_ov007_020ccffc, data_ov007_020ce564, data_ov007_020ce698, data_ov007_020cd22c,
    data_ov007_020d0060, data_ov007_020cea74, data_ov007_020ceda0, data_ov007_020cd99c,
    data_ov007_020cdf94, data_ov007_020ccfdc, data_ov007_020ccfec, data_ov007_020cdd3c,
    data_ov007_020cdd58, data_ov007_020cd220,
};
const unsigned char *data_ov007_020d8164[32] = {
    data_ov007_020cdccc, data_ov007_020cd394, data_ov007_020cce7c, data_ov007_020cd074,
    data_ov007_020ccd74, data_ov007_020ccd10, data_ov007_020cd5ec, data_ov007_020cd13c,
    data_ov007_020ce974, data_ov007_020cd1cc, data_ov007_020cd0ac, data_ov007_020cdf14,
    data_ov007_020cd464, data_ov007_020cd1e4, data_ov007_020cd8f4, data_ov007_020cdc94,
    data_ov007_020cce64, data_ov007_020cd7b8, data_ov007_020cce24, data_ov007_020ccd78,
    data_ov007_020ce1dc, data_ov007_020cce5c, data_ov007_020cce34, data_ov007_020cd05c,
    data_ov007_020cd07c, data_ov007_020ccf6c, data_ov007_020ccdfc, data_ov007_020cd190,
    data_ov007_020cd024, data_ov007_020ccf74, data_ov007_020ccdf4, data_ov007_020ccf2c,
};
const unsigned char *data_ov007_020d81e4[32] = {
    data_ov007_020cdce8, data_ov007_020cd3a4, data_ov007_020ccddc, data_ov007_020cd06c,
    data_ov007_020ccdb4, data_ov007_020ccd48, data_ov007_020cd600, data_ov007_020cd148,
    data_ov007_020ce9b4, data_ov007_020cd1d8, data_ov007_020cd0b8, data_ov007_020cdf34,
    data_ov007_020cd454, data_ov007_020cd1f0, data_ov007_020cd90c, data_ov007_020cdcb0,
    data_ov007_020ccdc4, data_ov007_020cd7cc, data_ov007_020cce4c, data_ov007_020ccd6c,
    data_ov007_020ce204, data_ov007_020cce44, data_ov007_020cce8c, data_ov007_020cd02c,
    data_ov007_020cd094, data_ov007_020ccfac, data_ov007_020cce54, data_ov007_020cd16c,
    data_ov007_020ccfc4, data_ov007_020ccf9c, data_ov007_020ccdbc, data_ov007_020ccf1c,
};
const int *data_ov007_020d8264[32] = {
    data_ov007_020cf5ac, data_ov007_020ce774, data_ov007_020cd704, data_ov007_020cdfb4,
    data_ov007_020cd484, data_ov007_020cd214, data_ov007_020cebd0, data_ov007_020ce2cc,
    data_ov007_020cff64, data_ov007_020ce3f0, data_ov007_020ce104, data_ov007_020cf870,
    data_ov007_020ce8fc, data_ov007_020ce41c, data_ov007_020cf178, data_ov007_020cf544,
    data_ov007_020cd7a4, data_ov007_020cef30, data_ov007_020cd7f4, data_ov007_020cd4b4,
    data_ov007_020cfc18, data_ov007_020cd808, data_ov007_020cd81c, data_ov007_020ce054,
    data_ov007_020ce194, data_ov007_020cde38, data_ov007_020cd844, data_ov007_020ce344,
    data_ov007_020cdf74, data_ov007_020cdd04, data_ov007_020cd6b4, data_ov007_020cdd20,
};
const unsigned char *data_ov007_020d82e4[32] = {
    data_ov007_020cdccc, data_ov007_020cd394, data_ov007_020cce7c, data_ov007_020cd074,
    data_ov007_020ccd74, data_ov007_020ccd10, data_ov007_020cd5ec, data_ov007_020cd13c,
    data_ov007_020ce974, data_ov007_020cd1cc, data_ov007_020cd0ac, data_ov007_020cdf14,
    data_ov007_020cd464, data_ov007_020cd1e4, data_ov007_020cd8f4, data_ov007_020cdc94,
    data_ov007_020cce64, data_ov007_020cd7b8, data_ov007_020cce24, data_ov007_020ccd78,
    data_ov007_020cdd74, data_ov007_020ce36c, data_ov007_020cdbec, data_ov007_020ccd18,
    data_ov007_020cccac, data_ov007_020ccda8, data_ov007_020ccebc, data_ov007_020ccf14,
    data_ov007_020cce2c, data_ov007_020cce0c, data_ov007_020ccd84, data_ov007_020ccd40,
};
const unsigned char *data_ov007_020d8364[32] = {
    data_ov007_020cdce8, data_ov007_020cd3a4, data_ov007_020ccddc, data_ov007_020cd06c,
    data_ov007_020ccdb4, data_ov007_020ccd48, data_ov007_020cd600, data_ov007_020cd148,
    data_ov007_020ce9b4, data_ov007_020cd1d8, data_ov007_020cd0b8, data_ov007_020cdf34,
    data_ov007_020cd454, data_ov007_020cd1f0, data_ov007_020cd90c, data_ov007_020cdcb0,
    data_ov007_020ccdc4, data_ov007_020cd7cc, data_ov007_020cce4c, data_ov007_020ccd6c,
    data_ov007_020cdd90, data_ov007_020ce398, data_ov007_020cdc08, data_ov007_020ccd28,
    data_ov007_020cccb4, data_ov007_020ccd8c, data_ov007_020ccecc, data_ov007_020cceac,
    data_ov007_020cce04, data_ov007_020cce3c, data_ov007_020ccd9c, data_ov007_020ccd24,
};
const int *data_ov007_020d83e4[32] = {
    data_ov007_020cf5ac, data_ov007_020ce774, data_ov007_020cd704, data_ov007_020cdfb4,
    data_ov007_020cd484, data_ov007_020cd214, data_ov007_020cebd0, data_ov007_020ce2cc,
    data_ov007_020cff64, data_ov007_020ce3f0, data_ov007_020ce104, data_ov007_020cf870,
    data_ov007_020ce8fc, data_ov007_020ce41c, data_ov007_020cf178, data_ov007_020cf544,
    data_ov007_020cd7a4, data_ov007_020cef30, data_ov007_020cd7f4, data_ov007_020cd4b4,
    data_ov007_020cf618, data_ov007_020cfd44, data_ov007_020cf40c, data_ov007_020cd244,
    data_ov007_020ccd70, data_ov007_020cd494, data_ov007_020cda74, data_ov007_020cda8c,
    data_ov007_020cd77c, data_ov007_020cd7e0, data_ov007_020cd4c4, data_ov007_020cd280,
};
const unsigned char *data_ov007_020d8464[34] = {
    data_ov007_020cd924, data_ov007_020cd100, data_ov007_020cce1c, data_ov007_020cce6c,
    data_ov007_020cccfc, data_ov007_020cd474, data_ov007_020cd1c0, data_ov007_020ccdb0,
    data_ov007_020cccbc, data_ov007_020ccfbc, data_ov007_020ccefc, data_ov007_020cdb7c,
    data_ov007_020cdbb4, data_ov007_020ccd2c, data_ov007_020cd8ac, data_ov007_020cd298,
    data_ov007_020cce74, data_ov007_020cd954, data_ov007_020cd118, data_ov007_020ccd94,
    data_ov007_020cd434, data_ov007_020cd9cc, data_ov007_020cde54, data_ov007_020ce22c,
    data_ov007_020cd6c8, data_ov007_020cd208, data_ov007_020cd324, data_ov007_020cd19c,
    data_ov007_020ce074, data_ov007_020cde94, data_ov007_020cd5d8, data_ov007_020ccca4,
    data_ov007_020ccca8, data_ov007_020ccd04,
};
const unsigned char *data_ov007_020d84ec[34] = {
    data_ov007_020cd93c, data_ov007_020cd0f4, data_ov007_020cce84, data_ov007_020ccde4,
    data_ov007_020ccce4, data_ov007_020cd444, data_ov007_020cd1b4, data_ov007_020ccd80,
    data_ov007_020cccc8, data_ov007_020ccfd4, data_ov007_020cceec, data_ov007_020cdb98,
    data_ov007_020cdbd0, data_ov007_020ccd14, data_ov007_020cd894, data_ov007_020cd1fc,
    data_ov007_020ccdcc, data_ov007_020cd96c, data_ov007_020cd10c, data_ov007_020ccd88,
    data_ov007_020cd384, data_ov007_020cd984, data_ov007_020cde74, data_ov007_020ce254,
    data_ov007_020cd6dc, data_ov007_020cd28c, data_ov007_020cd304, data_ov007_020cd1a8,
    data_ov007_020ce098, data_ov007_020cdeb4, data_ov007_020cd59c, data_ov007_020cccb0,
    data_ov007_020cccc0, data_ov007_020cccec,
};
const int *data_ov007_020d8574[34] = {
    data_ov007_020cf1d0, data_ov007_020ce1b8, data_ov007_020cd858, data_ov007_020cd86c,
    data_ov007_020cd064, data_ov007_020ce938, data_ov007_020ce448, data_ov007_020cd514,
    data_ov007_020ccd64, data_ov007_020cdf54, data_ov007_020cdaec, data_ov007_020cf344,
    data_ov007_020cf3a8, data_ov007_020cd2a4, data_ov007_020cefd0, data_ov007_020ce474,
    data_ov007_020cd6a0, data_ov007_020cf228, data_ov007_020ce27c, data_ov007_020cd4a4,
    data_ov007_020ce704, data_ov007_020cf284, data_ov007_020cf688, data_ov007_020cfcac,
    data_ov007_020cedf0, data_ov007_020ce4a4, data_ov007_020ce594, data_ov007_020ce3c4,
    data_ov007_020cf9ec, data_ov007_020cf778, data_ov007_020ceb40, data_ov007_020ccda4,
    data_ov007_020ccda0, data_ov007_020cd004,
};
const unsigned char *data_ov007_020d85fc[37] = {
    data_ov007_020cd5b0, data_ov007_020ccf54, data_ov007_020cce14, data_ov007_020cd424,
    data_ov007_020ccf04, data_ov007_020ccd50, data_ov007_020cd124, data_ov007_020ccf44,
    data_ov007_020cd034, data_ov007_020cd718, data_ov007_020cdc24, data_ov007_020cd830,
    data_ov007_020cd8c4, data_ov007_020ccea4, data_ov007_020ccd30, data_ov007_020cd414,
    data_ov007_020ccff4, data_ov007_020cd754, data_ov007_020cded4, data_ov007_020ccd20,
    data_ov007_020cdc5c, data_ov007_020cd3e4, data_ov007_020cd614, data_ov007_020cd088,
    data_ov007_020ccfa4, data_ov007_020ce0bc, data_ov007_020cd0dc, data_ov007_020cd54c,
    data_ov007_020cd63c, data_ov007_020ccdec, data_ov007_020ccedc, data_ov007_020cd0e8,
    data_ov007_020ccf84, data_ov007_020cced4, data_ov007_020cd3f4, data_ov007_020cd044,
    data_ov007_020cccc4,
};
const unsigned char *data_ov007_020d8690[37] = {
    data_ov007_020cd5c4, data_ov007_020ccf24, data_ov007_020cce94, data_ov007_020cd3b4,
    data_ov007_020ccf0c, data_ov007_020ccd34, data_ov007_020cd130, data_ov007_020ccf4c,
    data_ov007_020cd00c, data_ov007_020cd72c, data_ov007_020cdc40, data_ov007_020cd740,
    data_ov007_020cd8dc, data_ov007_020ccec4, data_ov007_020ccd4c, data_ov007_020cd3c4,
    data_ov007_020cd014, data_ov007_020cd768, data_ov007_020cdef4, data_ov007_020ccd38,
    data_ov007_020cdc78, data_ov007_020cd3d4, data_ov007_020cd628, data_ov007_020cd0c4,
    data_ov007_020ccf3c, data_ov007_020ce0e0, data_ov007_020cd0d0, data_ov007_020cd560,
    data_ov007_020cd650, data_ov007_020ccdd4, data_ov007_020ccee4, data_ov007_020cd0a0,
    data_ov007_020ccf7c, data_ov007_020ccef4, data_ov007_020cd404, data_ov007_020cd01c,
    data_ov007_020cccb8,
};
const int *data_ov007_020d8724[37] = {
    data_ov007_020ceb88, data_ov007_020cddac, data_ov007_020cd6f0, data_ov007_020ce7ac,
    data_ov007_020cd9fc, data_ov007_020cd238, data_ov007_020ce2a4, data_ov007_020cddc8,
    data_ov007_020cdfd4, data_ov007_020cee40, data_ov007_020cf474, data_ov007_020cee90,
    data_ov007_020cf120, data_ov007_020cda44, data_ov007_020cd250, data_ov007_020ce7e4,
    data_ov007_020cdff4, data_ov007_020ceee0, data_ov007_020cf7f4, data_ov007_020cd25c,
    data_ov007_020cf4dc, data_ov007_020ce81c, data_ov007_020cec1c, data_ov007_020ce128,
    data_ov007_020cdde4, data_ov007_020cfaf8, data_ov007_020ce14c, data_ov007_020ceab8,
    data_ov007_020cec68, data_ov007_020cd790, data_ov007_020cdaa4, data_ov007_020ce170,
    data_ov007_020cde00, data_ov007_020cdad4, data_ov007_020ce88c, data_ov007_020ce014,
    data_ov007_020ccd98,
};
