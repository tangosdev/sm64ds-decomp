#ifndef PRIVATE_OV007_PICKER_SCENE_H
#define PRIVATE_OV007_PICKER_SCENE_H

#include "types.h"
#include "Ov007MenuState.h"
#include "Ov007Picker.h"
#include "Ov007Tween.h"

/* data_ov007_02104ba0: the 0x4c-byte scene record func_ov007_020b9880
 * allocates. */
typedef struct Ov007PickerScene {
    Ov007Picker *picker; /* 0x00: func_ov007_020c690c; freed by func_ov007_020c6684 */
    /* 0x04: func_ov007_020c9388; [0] runs the scene, [1] the display, [2]
     * and [3] the picker's animations */
    Ov007MenuState *states[4];
    /* 0x14: func_ov007_020c5db0's point total over the stroke and every
     * group; func_ov007_020ba368 divides the drawn progress by it */
    int totalPoints;
    /* 0x18, medium confidence: func_ov007_020c5d5c's sum of each group's
     * last progress threshold, i.e. the drawing's whole stroke time */
    int totalStrokeTime;
    /* 0x1c: which six-int row of data_ov007_020d7b80 func_ov007_020ba9bc
     * installs as picker->springConstants */
    int springSet;
    /* 0x20: steps 0..4 each time the intro starts and picks its shape:
     * 0/3 squash along x/y, 1/4 spring pull, 2 wobble */
    int introVariant;
    int restartCount; /* 0x24: func_ov007_020ba2e0 counts each restart */
    /* 0x28: frames with no touch and no key; func_ov007_020b8fb4 tests
     * >= 0xf0, func_ov007_020b9f0c presets 0xf0 */
    int idleFrames;
    int strokeCount; /* 0x2c: picker->groupCount once a stroke is committed; redo stops at it */
    /* 0x30: set while the board waits for a fresh tap; the next fresh press
     * clears it and resets the picker (func_ov007_020ba05c) */
    int awaitingTap;
    /* 0x34: Q12 frames since the stroke began; func_ov007_020c65bc stores
     * it >> 12 as the new point's progress threshold */
    int strokeTime;
    /* 0x38, medium confidence: the frames a replay of the drawing takes;
     * func_ov007_020bbe60 sets it when it loads a portrait drawing,
     * func_ov007_020b9880 clears it */
    int replayFrames;
    s32 tweenOut[2];       /* 0x3c: bound as tweens[i]->out */
    Ov007Tween *tweens[2]; /* 0x44: func_ov007_020c908c */
} Ov007PickerScene;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007PickerScene_size_must_be_0x4c[sizeof(Ov007PickerScene) == 0x4c ? 1 : -1];
#endif

#endif
