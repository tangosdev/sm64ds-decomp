#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
    void *f010;
    void *f014;
    void *f018;
    void *f01c;
    void *f020;
    void *f024;
} data_ov030_02114824_Layout;
typedef char data_ov030_02114824_size[sizeof(data_ov030_02114824_Layout) == 0x28 ? 1 : -1];

extern const data_ov030_02114824_Layout data_ov030_02114824;
extern u8 gMonkeyHoldAnimationFile[];
extern u8 gMonkeyJumpStAnimationFile[];
extern u8 gMonkeyJumpEdAnimationFile[];
extern u8 gMonkeyRunStAnimationFile[];
extern u8 gMonkeyRunAnimationFile[];
extern u8 gMonkeyRunEdAnimationFile[];
extern u8 data_ov030_02115cf0[];
extern u8 gMonkeyWait2AnimationFile[];
extern u8 gMonkeyWait3AnimationFile[];
extern u8 gMonkeyWait4AnimationFile[];

const data_ov030_02114824_Layout data_ov030_02114824 = {
    &gMonkeyHoldAnimationFile[0],  &gMonkeyJumpStAnimationFile[0], &gMonkeyJumpEdAnimationFile[0],
    &gMonkeyRunStAnimationFile[0], &gMonkeyRunAnimationFile[0],    &gMonkeyRunEdAnimationFile[0],
    &data_ov030_02115cf0[0],       &gMonkeyWait2AnimationFile[0],  &gMonkeyWait3AnimationFile[0],
    &gMonkeyWait4AnimationFile[0],
};
