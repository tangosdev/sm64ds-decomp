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
extern const int data_ov007_020cfb84[36];
extern const int data_ov007_020cef80[20];
extern const int data_ov007_020cf96c[32];
extern const int data_ov007_020cf8ec[32];
static const int *sPortraitStrokePointCounts[6] = {data_ov007_020cf700, data_ov007_020cfa70,
                                                   data_ov007_020cfb84, data_ov007_020cef80,
                                                   data_ov007_020cf96c, data_ov007_020cf8ec};
extern const unsigned char *const data_ov007_020d7ffc[30];
extern const unsigned char *const data_ov007_020d8464[34];
extern const unsigned char *const data_ov007_020d85fc[36];
extern const unsigned char *const data_ov007_020d7f0c[20];
extern const unsigned char *const data_ov007_020d82e4[32];
extern const unsigned char *const data_ov007_020d8164[32];
static const unsigned char *const *sPortraitStrokeXCoords[6] = {
    data_ov007_020d7ffc, data_ov007_020d8464, data_ov007_020d85fc,
    data_ov007_020d7f0c, data_ov007_020d82e4, data_ov007_020d8164};
extern const unsigned char *const data_ov007_020d8074[30];
extern const unsigned char *const data_ov007_020d84ec[34];
extern const unsigned char *const data_ov007_020d8690[36];
extern const unsigned char *const data_ov007_020d7f5c[20];
extern const unsigned char *const data_ov007_020d8364[32];
extern const unsigned char *const data_ov007_020d81e4[32];
static const unsigned char *const *sPortraitStrokeYCoords[6] = {
    data_ov007_020d8074, data_ov007_020d84ec, data_ov007_020d8690,
    data_ov007_020d7f5c, data_ov007_020d8364, data_ov007_020d81e4};
extern const int *const data_ov007_020d80ec[30];
extern const int *const data_ov007_020d8574[34];
extern const int *const data_ov007_020d8724[36];
extern const int *const data_ov007_020d7fac[20];
extern const int *const data_ov007_020d83e4[32];
extern const int *const data_ov007_020d8264[32];
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
    SCENE->replayFrames = 0x78;
    if (idx == 0 && StrokeStages()[idx] == 2) {
        int frames = FX_MUL(SCENE->replayFrames << 12, 0x1800);
        SCENE->replayFrames = frames >> 12;
    }
    StrokeStages()[idx]++;
    if (StrokeStages()[idx] >= 3)
        StrokeStages()[idx] = 0;
}
