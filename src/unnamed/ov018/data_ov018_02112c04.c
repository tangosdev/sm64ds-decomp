#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} data_ov018_02112c04_Layout;
typedef char data_ov018_02112c04_size[sizeof(data_ov018_02112c04_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
} data_ov018_02112c08_Layout;
typedef char data_ov018_02112c08_size[sizeof(data_ov018_02112c08_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
} data_ov018_02112c0c_Layout;
typedef char data_ov018_02112c0c_size[sizeof(data_ov018_02112c0c_Layout) == 0x8 ? 1 : -1];

extern const data_ov018_02112c04_Layout data_ov018_02112c04;
extern const data_ov018_02112c08_Layout data_ov018_02112c08;
extern const data_ov018_02112c0c_Layout data_ov018_02112c0c;
extern u8 gPenguinAngryTextureSequenceFile[];
extern u8 gPenguinWait1TextureSequenceFile[];
extern u8 gPenguinWait1AnimationFile[];
extern u8 gPenguinWalkAnimationFile[];

const data_ov018_02112c08_Layout data_ov018_02112c08 = {
    &gPenguinWait1TextureSequenceFile[0],
};

const data_ov018_02112c04_Layout data_ov018_02112c04 = {
    &gPenguinAngryTextureSequenceFile[0],
};

const data_ov018_02112c0c_Layout data_ov018_02112c0c = {
    &gPenguinWait1AnimationFile[0],
    &gPenguinWalkAnimationFile[0],
};
