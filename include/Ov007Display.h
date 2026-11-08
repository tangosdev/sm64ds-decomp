#ifndef OV007_DISPLAY_H
#define OV007_DISPLAY_H

#include "types.h"
#include "Ov007BgLayer.h"
#include "Ov007OamNode.h"
#include "Ov007SpriteBank.h"
#include "Ov007Transition.h"

/* Reconstructed layouts and names, not recovered class identity: the ov007
 * menu's 2D display record and the graphics records its tables hold. */

/* The 8-byte palette record func_ov007_020c3398 allocates and
 * func_ov007_020c9988 fills from a palette file. func_ov007_020c32e8 loads
 * its colours into palette RAM; func_ov007_020c338c frees it. */
typedef struct Ov007Palette {
    const u16 *colors; /* 0x00: BGR555 colours, 16-colour (0x20-byte) rows */
    u16 colorCount;    /* 0x04 */
    u8 reserved;       /* 0x06: cleared by func_ov007_020c3398; nothing in ov007 reads it */
} Ov007Palette;

/* The 0xc-byte character record func_ov007_020c3550 allocates and
 * func_ov007_020c99d8 fills from a character file. func_ov007_020c33d0
 * loads its tiles into VRAM; func_ov007_020c3544 frees it. */
typedef struct Ov007CharRecord {
    const void *tiles; /* 0x00 */
    int is8bpp;        /* 0x04: 0x40-byte tiles when set, else 0x20 */
    u8 width;          /* 0x08: in tiles */
    u8 height;         /* 0x09 */
    u8 reserved;       /* 0x0a: cleared by func_ov007_020c3550; nothing in ov007 reads it */
} Ov007CharRecord;

/* The 0x48-byte display record func_ov007_020b7594 builds: the manager's
 * +0x74 (Ov007Work::display). Each table's count sits in the word before
 * it, and func_ov007_020b73b4 frees every table with its own record's free.
 * func_ov007_020b7320 (characters, palettes), func_ov007_020b72d8 (BG
 * layers) and func_ov007_020b72fc (sprite banks) size four of the six
 * tables, and func_ov007_020b6d40 fills them. Nothing in ov007 sizes or
 * fills transitions or oamNodes; func_ov007_020b7594 zeroes them. Their
 * record types are inferred from func_ov007_020b73b4's frees. It frees
 * transitions with Transition_Destroy, the free ScreenFade_Destroy uses
 * on Transition_Create's record. It frees oamNodes with
 * func_ov007_020c1180, the OAM node free func_ov007_020ade14 also uses. */
typedef struct Ov007Display {
    /* 0x00/0x04: the OAM node lists (func_ov007_020c1068) drawn into the
     * main and sub OAM buffers; func_ov007_020c0af4 appends a node */
    Ov007OamList *oamListMain;
    Ov007OamList *oamListSub;
    /* 0x08/0x0c: func_ov007_020c0604's BG layer lists for engine bit 1
     * (main) and 2 (sub); func_ov007_020c0384 appends a layer */
    Ov007BgList *bgListMain;
    Ov007BgList *bgListSub;
    /* 0x10/0x14: the 0x400-byte OAM buffers func_ov007_020b7384 copies to
     * the main and sub OAM */
    void *oamMain;
    void *oamSub;
    int transitionCount;           /* 0x18 */
    Ov007Transition **transitions; /* 0x1c: Transition_Create records */
    int characterCount;            /* 0x20 */
    Ov007CharRecord **characters;  /* 0x24: func_ov007_020c99d8 records */
    int paletteCount;              /* 0x28 */
    Ov007Palette **palettes;       /* 0x2c: func_ov007_020c9988 records */
    int bgLayerCount;              /* 0x30 */
    Ov007BgLayer **bgLayers;       /* 0x34: func_ov007_020c95fc layers */
    int spriteBankCount;           /* 0x38 */
    Ov007SpriteBank **spriteBanks; /* 0x3c: func_ov007_020c9688 banks */
    int oamNodeCount;              /* 0x40 */
    Ov007OamNode **oamNodes;       /* 0x44: func_ov007_020c11ac nodes */
} Ov007Display;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007Palette_size_must_be_0x8[sizeof(Ov007Palette) == 0x8 ? 1 : -1];
typedef char Ov007CharRecord_size_must_be_0xc[sizeof(Ov007CharRecord) == 0xc ? 1 : -1];
typedef char Ov007Display_size_must_be_0x48[sizeof(Ov007Display) == 0x48 ? 1 : -1];
#endif

#endif
