#include "nitro/fs/overlay.h"
#include "private/fs_views.h"

typedef struct FSFile FSFile;

/* NitroSDK CARDRomRegion. */
struct CARDRomRegion {
    u32 offset;
    u32 length;
};

extern void FS_InitFile(void *s);
extern int myFS_OpenFileDirect(char *p_file, FSArchive *p_arc, u32 image_top, u32 image_bottom,
                               u32 file_index);
extern int myFS_ReadFile(void *s, void *b, int c);
extern int FS_CloseFile(void *self);
/* The ARM9 and ARM7 overlay tables when they are in memory: {address,
   length}, address 0 while not loaded. */
extern struct CARDRomRegion data_020a8064; /* fsi_ovt9 */
extern struct CARDRomRegion data_020a806c; /* fsi_ovt7 */
/* the archive func_02018cbc reads the table out of */
extern FSArchive data_020a8074; /* fsi_arc_rom */
extern void CpuCopy8(const void *src, void *dst, unsigned int size);

/* The card half of func_02018c00: read overlay table entry index (0x20
   bytes) straight out of archive's ROM, the table offset and length coming
   from the cartridge header (ovt9Offset/ovt9Size or ovt7Offset/ovt7Size,
   header +0x50..+0x5c).
   0 past the end. */
int func_02018cbc(FSOverlayInfo *info, int flag, int index, FSArchive *archive, u32 ovt9Offset,
                  u32 ovt9Size, u32 ovt7Offset, u32 ovt7Size) {
    FSFile file;
    u32 off;
    u32 base;
    u32 size;
    if (flag == 0) {
        size = ovt9Size;
        base = ovt9Offset;
    } else {
        size = ovt7Size;
        base = ovt7Offset;
    }
    off = index << 5;
    if (off >= size)
        return 0;
    FS_InitFile(&file);
    myFS_OpenFileDirect((char *)&file, archive, base + off, base + size, -1);
    myFS_ReadFile(&file, &info->header, 0x20);
    FS_CloseFile(&file);
    info->target = flag;
    return 1;
}

/* The game's copy of NitroSDK FS_LoadOverlayInfo(info, target, id), which
   LoadOverlay, UnloadOverlay, func_02017e94 and FS_LoadOverlay call: fill
   the 0x24-byte FSOverlayInfo with overlay `index`'s 0x20-byte table entry,
   then the target processor (0 ARM9, 1 ARM7) at +0x20. The entry comes from the
   in-memory table if there is one, else from the card, with the table's ROM
   offset and length from the cartridge header copy at 0x27ffe00 (+0x50 ARM9,
   +0x58 ARM7). 0 if index is past the table. */
int func_02018c00(FSOverlayInfo *info, int flag, int index) {
    struct CARDRomRegion *base = (flag == 0) ? &data_020a8064 : &data_020a806c;
    u32 ptr = base->offset;
    if (ptr != 0) {
        u32 off = index << 5;
        if (off >= base->length)
            return 0;
        CpuCopy8((void *)(ptr + off), &info->header, 0x20);
        info->target = flag;
        return 1;
    }
    {
        struct CARDRomRegion *m = (struct CARDRomRegion *)0x27ffe50;
        struct CARDRomRegion *n = (struct CARDRomRegion *)0x27ffe58;
        return func_02018cbc(info, flag, index, &data_020a8074, m->offset, m->length, n->offset,
                             n->length);
    }
}
