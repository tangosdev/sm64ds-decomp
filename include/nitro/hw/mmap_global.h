#ifndef NITRO_HW_MMAP_GLOBAL_H
#define NITRO_HW_MMAP_GLOBAL_H

/* NitroSDK's ARM9 memory map (nitro/hw/ARM9/mmap_global.h): the palette and
 * OAM blocks of both display engines; the DB_ names are the sub engine's. */
#define HW_PLTT        0x05000000
#define HW_BG_PLTT     0x05000000
#define HW_OBJ_PLTT    0x05000200
#define HW_DB_BG_PLTT  0x05000400
#define HW_DB_OBJ_PLTT 0x05000600

#define HW_OAM    0x07000000
#define HW_DB_OAM 0x07000400

#endif
