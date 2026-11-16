#include "types.h"
#include "Ov007BgLayer.h"
#include "Ov007BinFile.h"

/* Block 0 of a screen file: a 16-byte header, then the screen data. The
 * whole header is copied before the two sizes are read. */
typedef struct Ov007ScreenBlock {
    int width;  /* 0x00: in tiles; becomes the BG layer's width */
    int height; /* 0x04: in tiles */
    int w8;
    int wc;
} Ov007ScreenBlock;

extern Ov007BgLayer *func_ov007_020c0aa8(void *screen, int bg, int priority, u16 width, u16 height);

/* Builds a BG layer around file a's screen (a blank 0x20 x 0x18-tile layer
 * when there is no file). */
Ov007BgLayer *func_ov007_020c95fc(int a, int b, int c) {
    Ov007ScreenBlock f;
    Ov007BinFile file;
    Ov007ScreenBlock *p;

    if (a == 0) {
        return func_ov007_020c0aa8(0, b, c, 32, 24);
    }
    func_ov007_020c9a2c(&file, a);
    p = (Ov007ScreenBlock *)file.blockData[0];
    f = *p;
    return func_ov007_020c0aa8(p + 1, b, c, (u16)f.width, (u16)f.height);
}
