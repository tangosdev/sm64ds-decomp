/* func_ov007_020c9688 -- ov007 0x020c9688, 0x300 bytes.
 *
 * Builds a sprite bank from a sprite file (func_ov007_020b6d40 builds six
 * into the display record's spriteBanks). The block index
 * func_ov007_020c9a2c fills carries two counted lists, list A in
 * blockData[0] and list B in blockData[3] (index offsets 0x44 and 0x50),
 * each a count word followed by its entries; the bank func_ov007_020c12ac
 * allocates carries the two output tables.
 *
 * The first pass walks list A, the cells. Each cell's header word gives
 * the number of 0x14-byte OAM records that follow it; each record is
 * packed into the three OAM attribute halfwords: attr0/attr1 from the
 * x/y halfwords and the affine, mode, mosaic, colour-mode, shape, size and
 * flip bytes (the affine index only for affine entries, the colour-mode bit
 * not for mode 3, bitmap OBJs), attr2 from the tile (0x10), priority (0xe)
 * and palette (0xf).
 *
 * The second pass walks list B, the cell animations. Each 0xc-byte header
 * gives a frame count at +8; func_ov007_020c13a4 sizes the animation, then
 * every 0x18-byte frame contributes a cell (index at +0) and a duration
 * (+2). */
#include "Ov007SpriteBank.h"
#include "Ov007BinFile.h"
#include "Ov007MenuMem.h"
#include "nitro/gx/gxcommon.h"

/* A count word followed by byte-oriented entries; each list chooses the
 * entry format. */
typedef struct Ov007CountedList {
    u32 count;
    u8 firstEntry;
} Ov007CountedList;

/* List A: a cell's 4-byte header, then one 0x14-byte record per OAM entry.
 * The record names are the OAM attribute fields each byte lands in. */
typedef struct Ov007OamRecord {
    s16 x;          /* 0x00: attr1 bits 0-8 */
    s16 y;          /* 0x02: attr0 bits 0-7 */
    u8 pad_04;      /* 0x04 */
    u8 affineIndex; /* 0x05: attr1 bits 9-13, affine entries only */
    u8 hFlip;       /* 0x06: attr1 bit 12 */
    u8 vFlip;       /* 0x07: attr1 bit 13 */
    u8 affineMode;  /* 0x08: attr0 bits 8-9 (1 affine, 3 double-size affine) */
    u8 objMode;     /* 0x09: attr0 bits 10-11 (3 bitmap) */
    u8 mosaic;      /* 0x0a: attr0 bit 12 */
    u8 colorMode;   /* 0x0b: attr0 bit 13, not for bitmap entries */
    u8 shape;       /* 0x0c: attr0 bits 14-15 */
    u8 size;        /* 0x0d: attr1 bits 14-15 */
    u8 priority;    /* 0x0e: attr2 bits 10-11 */
    u8 palette;     /* 0x0f: attr2 bits 12-15 */
    u16 tile;       /* 0x10: attr2 bits 0-9 */
    u16 pad_12;     /* 0x12 */
} Ov007OamRecord;

/* List B: a 0xc-byte cell-animation header, then `frameCount` 0x18-byte
 * frames. Only the slots this function reads are named. */
typedef struct Ov007CellAnimHeader {
    u8 pad_00[8];
    u8 frameCount; /* 0x08: frames that follow; func_ov007_020c13a4 sizes the animation */
    u8 pad_09[3];
} Ov007CellAnimHeader;

typedef struct Ov007CellAnimFrame {
    u16 cell;     /* 0x00: index into the bank's cells */
    u16 duration; /* 0x02: ticks */
    u8 pad_04[0x14];
} Ov007CellAnimFrame;

typedef char Ov007CellAnimHeader_size_must_be_0xc[sizeof(Ov007CellAnimHeader) == 0xc ? 1 : -1];
typedef char Ov007CellAnimFrame_size_must_be_0x18[sizeof(Ov007CellAnimFrame) == 0x18 ? 1 : -1];
typedef char Ov007OamRecord_size_must_be_0x14[sizeof(Ov007OamRecord) == 0x14 ? 1 : -1];

extern Ov007SpriteBank *func_ov007_020c12ac(int cellCount, int animCount);
extern void func_ov007_020c13a4(Ov007CellAnim *anim, int frameCount, u16 offsetX, u16 offsetY);

Ov007SpriteBank *func_ov007_020c9688(int arg) {
    Ov007SpriteBank *s;
    u32 i;
    u32 nA;
    u32 nB;
    u32 cnt;
    const char *name = 0;
    GXOamAttr attrs;
    Ov007BinFile file;
    u32 j;
    u8 *p;

    func_ov007_020c9a2c(&file, arg);
    nA = ((Ov007CountedList *)file.blockData[0])->count;
    nB = ((Ov007CountedList *)file.blockData[3])->count;
    s = func_ov007_020c12ac(nA, nB);

    p = &((Ov007CountedList *)file.blockData[0])->firstEntry;
    for (i = 0; i < nA; i++) {
        Ov007Cell *cell;
        cnt = *p;
        p = p + 4;
        cell = &s->cells[i];
        cell->oam = func_ov007_020c3df4(name, cnt * 6);
        cell->count = cnt;
        for (j = 0; j < cnt; j++) {
            s16 x;
            u32 modeFlip;
            u32 shapeSize;
            u8 priority;
            u32 tile;
            u8 objMode;
            u8 mosaic;
            u8 colorMode;
            u32 palette;
            Ov007OamRecord *r;
            s16 y;
            u8 affineIndex;
            r = (Ov007OamRecord *)p;
            modeFlip = ((u32)r->affineMode << 8) | ((u32)r->hFlip << 28) | ((u32)r->vFlip << 29);
            shapeSize = ((u32)r->shape << 14) | ((u32)r->size << 30);
            p = p + sizeof(Ov007OamRecord);
            palette = r->palette;
            tile = r->tile;
            affineIndex = r->affineIndex;
            colorMode = r->colorMode;
            mosaic = r->mosaic;
            objMode = r->objMode;
            priority = r->priority;
            y = r->y;
            x = r->x;
            if (modeFlip == 0x100 || modeFlip == 0x300) {
                if (objMode == 3) {
                    attrs.attr01 = modeFlip | ((shapeSize | ((y & 0xff) | (affineIndex << 25) |
                                                             (objMode << 10) | (mosaic << 12))) |
                                               ((x & 0x1ff) << 16));
                } else {
                    attrs.attr01 =
                        modeFlip |
                        ((shapeSize | ((y & 0xff) | ((affineIndex << 25) | (colorMode << 13)) |
                                       (objMode << 10) | (mosaic << 12))) |
                         ((x & 0x1ff) << 16));
                }
            } else {
                if (objMode == 3) {
                    attrs.attr01 =
                        modeFlip | ((shapeSize | ((y & 0xff) | (objMode << 10) | (mosaic << 12))) |
                                    ((x & 0x1ff) << 16));
                } else {
                    attrs.attr01 = modeFlip | ((shapeSize | ((y & 0xff) | (colorMode << 13) |
                                                             (objMode << 10) | (mosaic << 12))) |
                                               ((x & 0x1ff) << 16));
                }
            }
            attrs.attr2 = tile | (priority << 10) | (palette << 12);
            cell->oam[j].attr0 = attrs.attr0;
            cell->oam[j].attr1 = attrs.attr1;
            cell->oam[j].attr2 = attrs.attr2;
        }
    }

    {
        u32 k;
        Ov007CellAnim *anim;
        u32 f;
        u32 frameCount;
        p = &((Ov007CountedList *)file.blockData[3])->firstEntry;
        for (k = 0; k < nB; k++) {
            frameCount = ((Ov007CellAnimHeader *)p)->frameCount;
            anim = &s->anims[k];
            p = p + sizeof(Ov007CellAnimHeader);
            func_ov007_020c13a4(anim, frameCount, 0, 0);
            for (f = 0; f < frameCount; f++) {
                Ov007CellAnimFrame *frame = (Ov007CellAnimFrame *)p;
                p = p + sizeof(Ov007CellAnimFrame);
                anim->frames[f] = &s->cells[frame->cell];
                anim->durations[f] = frame->duration;
            }
        }
    }
    return s;
}
