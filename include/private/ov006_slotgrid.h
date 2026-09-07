#ifndef OV006_SLOTGRID_H
#define OV006_SLOTGRID_H
typedef struct SlotGrid {
    unsigned char pad[20508]; /* 0x501c */
    unsigned char sym[3][5];
} SlotGrid;
#endif
