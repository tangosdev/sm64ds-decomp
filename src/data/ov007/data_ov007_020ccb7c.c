/* 0x020ccb54: the per-item animation indices, and at 0x020ccb7c the item node
 * table func_ov007_020ade58 reads (Ov007NodeSprites[9]). One record keeps the
 * ROM's address order: the eight arrays, then the nine table rows. 0x020ccb7d
 * is row 0's animCount byte, read by its own symbol. */
#include "types.h"
#include "Ov007Item.h"

const struct {
    u8 item0[4];      /* 0x020ccb54 */
    u8 item7[4];      /* 0x020ccb58 */
    u8 item4[4];      /* 0x020ccb5c */
    u8 item8[4];      /* 0x020ccb60 */
    u8 items3And6[4]; /* 0x020ccb64: shared by items 3 and 6 */
    u8 item2[4];      /* 0x020ccb68 */
    u8 item5[4];      /* 0x020ccb6c */
    u8 item1[12];     /* 0x020ccb70: item 1 has ten animations */
} data_ov007_020ccb54 = {
    {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00},
    {0x01, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00},
    {0x00, 0x00, 0x00, 0x00},
    {0x04, 0x00, 0x00, 0x00},
    {0x02, 0x00, 0x00, 0x00},
    {0x00, 0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0x00},
};

const Ov007NodeSprites data_ov007_020ccb7c[9] = {
    {5, 1, {0}, (u8 *)data_ov007_020ccb54.item0},
    {0, 10, {0}, (u8 *)data_ov007_020ccb54.item1},
    {0, 1, {0}, (u8 *)data_ov007_020ccb54.item2},
    {3, 1, {0}, (u8 *)data_ov007_020ccb54.items3And6},
    {3, 1, {0}, (u8 *)data_ov007_020ccb54.item4},
    {3, 1, {0}, (u8 *)data_ov007_020ccb54.item5},
    {3, 1, {0}, (u8 *)data_ov007_020ccb54.items3And6},
    {4, 1, {0}, (u8 *)data_ov007_020ccb54.item7},
    {0, 1, {0}, (u8 *)data_ov007_020ccb54.item8},
};
