#ifndef OV007_WORK_H
#define OV007_WORK_H

#include "types.h"

/* Reconstructed names and layout, not an original class identity.
 *
 * ov007's work block: func_ov007_020b68e8 allocates it,
 * func_ov007_020c3df4(data_ov007_02102dc4, 0x188), and stores it in
 * data_ov007_0210342c; func_ov007_020b65e0 frees it with func_ov007_020c3d1c.
 * Each field below was named by the files that use it; where they agreed on
 * a name but not a type, the typed pointer is the one kept. Words no file
 * names stay padding. */
typedef struct Ov007Work {
    /* func_ov007_020b7040 adds one per drawn frame */
    int frameCount; /* 0x000 */
    /* func_ov007_020c9388; func_ov007_020b65e0 frees them with func_ov007_020c937c */
    struct Ov007MenuState *states[3]; /* 0x004 */
    /* the choice func_ov007_020aec94 was last given; func_ov007_020b0834
     * replays it once the state-9 window closes */
    int pendingChoice; /* 0x010 */
    /* func_ov007_020b0548 sets it in main state 7, func_ov007_020b1604 and
     * func_ov007_020b6544 clear it; while it is set func_ov007_020b7090
     * returns pendingChoice and func_ov007_020b6f4c fades out to white */
    int choiceReady; /* 0x014 */
    /* func_ov007_020b6eb4 starts a display capture (func_ov007_020bd4e8
     * writes DISPCAPCNT) while it is set, and clears it */
    int pendingCapture; /* 0x018 */
    /* func_ov007_020b6eb4 hands it to func_ov007_020bd648 and clears it */
    int pendingBgMode; /* 0x01c */
    /* func_ov007_020b1718 runs mode 1 after a stylus tap, mode 0 after a button */
    int transitionMode; /* 0x020 */
    /* medium confidence: the preset func_ov007_020b1fa4 last applied */
    int cameraPreset; /* 0x024 */
    /* the 0x3c-byte record func_ov007_020b6a84 allocates */
    struct Ov007Settings *settings; /* 0x028 */
    /* func_ov007_020b7138 takes it from its caller's argument record; called
     * with a request id (0x12, 0x16, 0x17, ...), and func_ov007_020b0834
     * stores what ids 0x16 / 0x17 return into the settings */
    int (*callback)(int id); /* 0x02c */
    /* both built by func_ov007_020c023c; func_ov007_020b2160 loads [0] into
     * the hardware projection and computes [1] into its own matrices */
    struct Ov007Camera *cameras[2]; /* 0x030 */
    /* func_ov007_020c241c(0/1); func_ov007_020c232c writes them to
     * LIGHT_VECTOR / LIGHT_COLOR */
    struct Ov007Light *lights[2]; /* 0x038 */
    /* func_ov007_020bce70 requests palette n on it and blends over its stateTicks */
    struct Ov007MenuState *paletteState; /* 0x040 */
    /* func_ov007_020b69c4 allocates them with func_ov007_020c946c and chains
     * them; func_ov007_020bce70 blends palettes through them;
     * func_ov007_020b65e0 releases them with func_ov007_020c9460 */
    void *paletteBufs[3]; /* 0x044 */
    /* func_ov007_020c2068's 0x44-byte touch-panel record, updated by
     * func_ov007_020c1db0 */
    struct Ov007Touch *touch; /* 0x050 */
    struct Ov007Pad *pad;     /* 0x054 */
    /* medium confidence: the tapped slot and its target, -1 = none;
     * func_ov007_020af84c indexes data_ov007_020ccc20[selectedSlot][targetSlot] */
    int selectedSlot; /* 0x058 */
    int targetSlot;   /* 0x05c */
    /* left unnamed: func_ov007_020afc44 passes it as func_ov007_020b8188's
     * third argument; func_ov007_020b0244 zeroes it */
    u8 pad_060[4];
    /* bound as tweens[i]->out (func_ov007_020b6a84) */
    int tweenOut[2];              /* 0x064 */
    struct Ov007Tween *tweens[2]; /* 0x06c: func_ov007_020c908c */
    struct Ov007Display *display; /* 0x074: func_ov007_020b7594's record */
    /* func_ov007_020b697c builds them from the BMD textures */
    void *textures[11]; /* 0x078 */
    /* func_ov007_020b6d40, one per func_ov007_020ade58 */
    struct Ov007ItemNode *itemNodes[9]; /* 0x0a4 */
    /* func_ov007_020b6d40 stores func_ov007_020adc08(i, ...) for i = 0..7 */
    struct Ov007LayerSlot *layerSlots[8]; /* 0x0c8 */
    /* func_ov007_020b6a84 clears it and func_ov007_020b2998 sets it once the
     * model and screen sprites are built; func_ov007_020be850 picks no
     * figure while it is 0 */
    int modelReady; /* 0x0e8 */
    /* 0 or 1: which of figure / altFigure is shown (func_ov007_020be850
     * returns altFigure when it is 1); copied from nextFigureIndex */
    int figureIndex; /* 0x0ec */
    /* seeded by func_ov007_020b7138 from its flags' bit 0, toggled by
     * func_ov007_020b066c */
    int nextFigureIndex;      /* 0x0f0 */
    struct Ov007Model *model; /* 0x0f4: func_ov007_020b2998 stores func_ov007_020b2bd4's */
    /* Q12; func_ov007_020b2370 scales the model from 1/16 at 0 to 1 at
     * 0x1000; func_ov007_020b1b74 drives it past 0x1000 */
    int modelGrowth; /* 0x0f8 */
    /* per figureIndex: which of three portrait drawings func_ov007_020bbe60
     * strokes next (it steps 0, 1, 2, 0); seeded by func_ov007_020b7138 */
    u8 strokeStage[2]; /* 0x0fc */
    u8 pad_0fe[2];
    struct Ov007Figure *figure;    /* 0x100: func_ov007_020beaa0 */
    struct Ov007Figure *altFigure; /* 0x104 */
    /* the three full-screen quads func_ov007_020b2998 builds: [0] a
     * corner-colour fill, [1] textured, [2] plain white */
    struct Ov007Sprite *screenSprites[3]; /* 0x108 */
    /* func_ov007_020b693c, one per func_ov007_020b4c04 */
    struct Ov007ItemView *itemViews[0x18]; /* 0x114 */
    /* medium confidence: func_ov007_020b42d8 builds them; func_ov007_020b413c
     * draws a number with them */
    struct Ov007Sprite *numberSprites[3]; /* 0x174 */
    /* the id of the item chosen this frame, -1 = none */
    int chosenItemId; /* 0x180 */
    u8 pad_184[4];    /* func_ov007_020b68e8 clears this word after allocating */
} Ov007Work;

typedef char Ov007Work_size_must_be_0x188[sizeof(Ov007Work) == 0x188 ? 1 : -1];

#endif
