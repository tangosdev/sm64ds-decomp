//cpp
/* Game file management: the loaded-overlay slots, the LZ-tagged file
 * loaders, and the thirteen-NARC archive table machinery. One contiguous
 * run 0x02017e60..0x02018aa4, gapless in config/arm9/delinks.txt between
 * SharedFilePtr and func_02018aa4. mwccarm emits deferred bodies in reverse
 * source order, so the definitions run ROM-descending: the asset-handle
 * table allocator first, the overlay-slot reset last. */
#include "types.h"
#include "nitro/os/cache.h"
#include "nitro/os/system.h"
#include "nitro/mi/memory.h"
#include "nitro/fs/file.h"
#include "nitro/fs/overlay.h"
#include "private/fs_views.h"
#include "private/arm9_archive_table.h"
#include "msl/string.h"
#include "Memory.h"
#include "Heap.h"

extern "C" {

/* The 12 loaded-overlay slots LoadOverlay fills: the overlay id (-1 for a
   free slot) and the RAM range it occupies. */
struct OverlaySlot {
    int id, start, size;
};

extern int func_0203d7b8(void);
extern int myFS_OpenFileFast(FSFile *p_file, FSFileID id);
extern int myFS_OpenFile(FSFile *p_file, const char *path);
extern int myFS_ReadFile(FSFile *p_file, void *dst, int len);
extern int myFS_ConvertPathToFileID(FSFileID *p_file_id, const char *path);
extern unsigned func_02018ac4(unsigned *p);
extern void DecompressLZ16(void *src, void *dst);
extern int func_0204eda4(int file, char *archive, unsigned int index);
extern int func_0204ede8(void *o, unsigned int i);
extern int func_0204ee10(struct S82 *archive);
extern int func_0204ee40(void *archive, void *name, void *narc);
extern s32 GetOwnerLanguage(void);
extern void *data_020a0ea0;
extern int data_0208edf8[];

/* Bodies run ROM-descending for the reverse-emission order, so the two
   upward calls in the run see their callees declared ahead of use. */
FSFileID func_02018598(int arg);
int func_02018270(unsigned int handle, void *dest, int size);

/* The asset handle table count, the ROM archive func_020189f0 last resolved
   a path in, the asset handle table itself, the last file id any load
   touched (the crash screen prints it), one word read by myFS_ReadFile's
   caller, and the 12 loaded-overlay slots. */
int data_0209d3b0;
FSArchive *data_0209d3b4;
unsigned short *data_0209d3b8;
unsigned int gLastUsedFileID;
void *data_0209d3c0;
struct OverlaySlot gLoadedOverlays[12] = {0};

/* Allocate a zeroed asset handle table of n u16 file ids. Coined name. */
int AllocAssetHandleTable(int n) {
    unsigned int sz = n * 2;
    unsigned short *p = (unsigned short *)Memory::Allocate(sz);
    data_0209d3b8 = p;
    MI_CpuClear8(p, sz);
    data_0209d3b0 = n;
    return 1;
}

/* Record asset handle i's FAT file id (InitializeAssetHandleTable). Coined
   name. The id is taken as an int: the one caller passes its int file id
   unnarrowed, and an unsigned short parameter would add a narrowing pair at
   that call site that the ROM does not have. */
void SetAssetHandleFileID(unsigned int i, int fileId) {
    data_0209d3b8[i] = fileId;
}

/* The file id for asset handle i (below 0x8000); an id of 0x8000 or more
   (an archived file) passes through unchanged. */
unsigned int func_02018a24(unsigned int i) {
    return i < 0x8000 ? data_0209d3b8[i] : i;
}

/* Resolve a ROM path to its FAT file id, keeping the archive the FSFileID
   names for func_02018598. myFS_ConvertPathToFileID crashes on a path that
   does not resolve. */
int func_020189f0(const char *path) {
    FSFileID file_id;
    myFS_ConvertPathToFileID(&file_id, path);
    data_0209d3b4 = file_id.arc;
    return file_id.file_id;
}

/* LoadArchive's loader: read the .narc at path whole into one heap block,
   after the head (the FSArchive and the NARC, FATB and FIMG pointers), and
   mount it there under name (func_0204ee40). Returns the block, or 0
   (freeing it) if the read or the mount fails. */
enum { ARCHIVE_HEAD_SIZE = 0x60 };
void *func_02018934(char *name, char *path, void *heap) {
    FSFile file;
    void *result = 0;
    void *ptr;
    void *p60;
    int diff, aligned;

    FS_InitFile(&file);
    myFS_OpenFile(&file, path);
    diff = FS_GetLength(&file);
    aligned = (diff + 15) & ~15;
    ptr = ((mHeap::Heap_t *)heap)->_Allocate(aligned + ARCHIVE_HEAD_SIZE, -0x10);
    p60 = (char *)ptr + ARCHIVE_HEAD_SIZE;
    DC_FlushRange(p60, aligned);
    if (myFS_ReadFile(&file, p60, diff) == diff && func_0204ee40(ptr, name, p60)) {
        result = ptr;
    } else {
        ((mHeap::Heap_t *)heap)->_Deallocate(ptr);
    }
    FS_CloseFile(&file);
    return result;
}

/* Unmount the archive and give its block back to heap. */
void func_02018908(struct S82 *archive, void *heap) {
    func_0204ee10(archive);
    ((mHeap::Heap_t *)heap)->_Deallocate(archive);
}

/* Mount archive idx from its .narc on first use; nonzero once it is mounted. */
int LoadArchive(int idx) {
    ArchiveTableEntry *e = &gArchiveTable[idx];
    if (e->archive == 0) {
        e->heap = data_020a0ea0;
        e->archive = func_02018934(e->name, e->path, e->heap);
    }
    return e->archive != 0;
}

/* Give archive i back to the heap it came from, if it is mounted. */
void UnloadArchive(int i) {
    ArchiveTableEntry *p = &gArchiveTable[i];
    void *a = p->archive;
    if (a == 0)
        return;
    func_02018908((struct S82 *)a, p->heap);
    p->archive = 0;
    p->heap = 0;
}

void LoadTextNarcs(void) {
    s32 archiveID;
    s32 lang;

    if (!LoadArchive(6))
        return;

    archiveID = 8;
    lang = GetOwnerLanguage();
    switch (lang) {
    case 2:
        archiveID = 9;
        break;
    case 3:
        archiveID = 10;
        break;
    case 4:
        archiveID = 11;
        break;
    case 5:
        archiveID = 12;
        break;
    default:
        break;
    }
    LoadArchive(archiveID);
}

/* Unload every mounted archive. Unlike UnloadArchive it leaves heap set. */
void UnloadArchives(void) {
    unsigned int i;
    ArchiveTableEntry *p = gArchiveTable;
    for (i = 0; i < ARCHIVE_TABLE_COUNT; i++) {
        if (p->archive != 0) {
            func_02018908((struct S82 *)p->archive, p->heap);
            p->archive = 0;
        }
        p++;
    }
}

/* Unload the archive a file lookup mounted for itself, if there is one
   (data_0208eb54 holds its index), then set the index to -1 (none). */
void func_02018770(void) {
    if (data_0208eb54 < 0) {
        return;
    }
    UnloadArchive(data_0208eb54);
    data_0208eb54 = -1;
}

/* The address of archived file val's bytes inside its mounted archive
   (func_0204ede8), mounting the archive first if needed. 0 for a FAT id
   (below 0x8000), which has no in-memory copy. */
int func_020186c0(unsigned int val) {
    unsigned int i;
    ArchiveTableEntry *e;
    if (val < 0x8000)
        return 0;
    e = gArchiveTable;
    for (i = 0; i < ARCHIVE_TABLE_COUNT; i++) {
        if (e->firstFileId <= val && val < e->endFileId) {
            if (e->archive == 0) {
                if (LoadArchive(i))
                    data_0208eb54 = i;
                if (e->archive == 0) {
                    OS_Terminate();
                    return 0;
                }
            }
            return func_0204ede8(e->archive, val - e->firstFileId);
        }
        e++;
    }
    OS_Terminate();
    return 0;
}

/* Open game file id into the FSFile at file. Ids below 0x8000 are FAT ids
   in the ROM archive (func_02018598 builds the FSFileID); higher ids are
   opened inside the archive whose range holds them, mounting it first if
   needed (and recording it in data_0208eb54). Any failure is fatal. */
int func_020185c0(FSFile *file, unsigned int id) {
    int result = 0;

    FS_InitFile(file);

    if (id < 0x8000) {
        FSFileID arg;
        arg = func_02018598(id);
        result = myFS_OpenFileFast(file, arg);
    } else {
        unsigned int i = 0;
        ArchiveTableEntry *e = gArchiveTable;
        do {
            if (e->firstFileId <= id && id < e->endFileId) {
                if (e->archive == 0) {
                    if (LoadArchive(i) != 0)
                        data_0208eb54 = i;
                    if (e->archive == 0) {
                        OS_Terminate();
                        return 0;
                    }
                }
                result = func_0204eda4((int)file, (char *)e->archive, id - e->firstFileId);
                break;
            }
            i++;
            e++;
        } while (i < ARCHIVE_TABLE_COUNT);
    }

    if (result == 0)
        OS_Terminate();
    return result;
}

/* Build the FSFileID for FAT file id arg in the ROM archive. The struct
   comes back by value: the copy out of the return slot is the one place in
   the run where mwccarm keeps a fieldwise struct copy it would otherwise
   optimise away, which is how the ROM's shape pins the spelling. */
FSFileID func_02018598(int arg) {
    FSFileID t;
    t.file_id = arg;
    t.arc = data_0209d3b4;
    return t;
}

/* Whether the file bytes at a open with the "LZ77" tag (memcmp of the
   first 4 bytes). The loaders then read the LZ header at
   a + 4 (func_02018ac4) and decompress from there. The file address is an
   int here, as most of the callers declare it. */
int func_02018568(int a) {
    return memcmp((const void *)a, data_0208edf8, 4) == 0;
}

/* Read up to n bytes of the open file into buf (cache-maintained around the
   read), close it and unload any archive the open mounted. The length read,
   or 0 if the read came up short. The LZ77-tag test on the result is made
   and dropped: this path never decompresses. */
int func_020184e0(FSFile *self, void *buf, unsigned int n) {
    int r;
    unsigned int len = FS_GetLength(self);
    if (len > n)
        len = n;
    DC_FlushRange(buf, len);
    r = myFS_ReadFile(self, buf, len);
    FS_CloseFile(self);
    func_02018770();
    if (len != r)
        return 0;
    func_02018568((int)buf);
    DC_StoreRange(buf, len);
    return len;
}

/* Open the file fileId names (func_02018434 passes it by value), crash if
   it will not open, and read up to size bytes of it into dst
   (func_020184e0). */
int func_02018474(FSFileID fileId, void *dst, int size) {
    FSFile file;
    FS_InitFile(&file);
    if (myFS_OpenFileFast(&file, fileId) == 0) {
        OS_Terminate();
        return 0;
    }
    return func_020184e0(&file, dst, size);
}

/* Read up to size bytes of the ROM file at path into dst (func_02018474).
   func_0203ad84 reads "/BUILDTIME" this way. */
void func_02018434(const char *path, void *dst, int size) {
    FSFileID fileId;
    myFS_ConvertPathToFileID(&fileId, path);
    func_02018474(fileId, dst, size);
}

/* Read the whole open file (more than 8 bytes, or crash) into a new heap
   block: an LZ77-tagged file is decompressed into a block of its unpacked
   size, anything else copied. Closes the file, unloads any archive the open
   mounted, and with flush set flushes the result from the data cache. */
void *func_0201834c(FSFile *file, int flush) {
    void *result = 0;
    unsigned int len = FS_GetLength(file);
    if (len > 8) {
        u32 *buf = (u32 *)Memory::Allocate(len, -4);
        DC_FlushRange(buf, len);
        int readLen = myFS_ReadFile(file, buf, len);
        if (readLen != len)
            return result;
        if (func_02018568((int)buf) != 0) {
            /* the LZ header word follows the "LZ77" tag */
            len = func_02018ac4(buf + 1);
            result = Memory::Allocate(len);
            DecompressLZ16(buf + 1, result);
        } else {
            result = Memory::Allocate(len);
            MI_CpuCopy8(buf, result, len);
        }
        Memory::Deallocate(buf);
    } else {
        OS_Terminate();
    }
    FS_CloseFile(file);
    func_02018770();
    if (flush != 0)
        DC_StoreRange(result, len);
    return result;
}

/* Load the file an FSFileID names (see func_0201834c); flush nonzero
   flushes the result from the data cache. */
void *func_020182ec(FSFileID fileId, int flush) {
    FSFile file;
    void *result = 0;
    FS_InitFile(&file);
    if (myFS_OpenFileFast(&file, fileId) == 0)
        OS_Terminate();
    else
        result = func_0201834c(&file, flush);
    return result;
}

/* Load the ROM file at path into a new heap block, decompressed if it is
   LZ77-tagged, and flush it from the data cache (Ov004_LoadFileByPath). */
void *func_020182bc(const char *path) {
    FSFileID fileId;
    myFS_ConvertPathToFileID(&fileId, path);
    return func_020182ec(fileId, 1);
}

int LoadFileAt(int handle, void *dest) {
    return func_02018270(handle, dest, -1);
}

/* Read up to size bytes of the asset handle's file into dest (func_020184e0). */
int func_02018270(unsigned int handle, void *dest, int size) {
    char buf[0x44];
    unsigned int r = func_02018a24(handle);
    func_020185c0((FSFile *)buf, r);
    return func_020184e0((FSFile *)buf, dest, size);
}

/* Load file id (an asset handle, or an archived file id) into a new heap
   block, as SharedFilePtr::Load does: a file resident in a mounted archive
   is decompressed or copied out of it, anything else read through
   func_0201834c. flush nonzero flushes the result from the data cache. */
void *func_0201818c(unsigned int id, int flush) {
    FSFile obj;
    FSFile file;
    int p = func_020186c0(id);
    if (p != 0) {
        unsigned int size;
        void *mem;
        if (func_02018568(p)) {
            unsigned int *q = (unsigned int *)(p + 4);
            size = func_02018ac4(q);
            mem = Memory::Allocate(size);
            DecompressLZ16(q, mem);
        } else {
            func_020185c0(&obj, id);
            size = FS_GetLength(&obj);
            FS_CloseFile(&obj);
            mem = Memory::Allocate(size, 0x20);
            MI_CpuCopy8((const void *)p, mem, size);
        }
        func_02018770();
        if (flush != 0)
            DC_StoreRange(mem, size);
        return mem;
    }
    func_020185c0(&file, func_02018a24(id));
    return func_0201834c(&file, flush);
}

/* func_0201818c without the cache flush. */
void *func_0201817c(int handle) {
    return func_0201818c(handle, 0);
}

void *LoadFile(int handle) {
    return func_0201818c(handle, 1);
}

void Deallocate(void *ptr) {
    if (ptr)
        Memory::Deallocate(ptr);
}

u32 LoadCompressedFileAt(int fileID, void *target) {
    void *src;
    u32 size;

    src = func_0201817c(fileID);
    if (!src)
        return 0;

    size = func_02018ac4((unsigned *)src);
    DecompressLZ16(src, target);
    Deallocate(src);
    return size;
}

void LoadOverlay(int id) {
    u32 start;
    struct OverlaySlot *slot;
    u32 i;
    u32 total;
    FSFileID pair;
    FSOverlayInfo info;
    struct OverlaySlot *r;
    u32 end;
    func_02018c00(&info, MI_PROCESSOR_ARM9, id);
    if (func_0203d7b8() != 0) {
        return;
    }
    pair = FS_GetOverlayFileID(&info);
    slot = 0;
    total = info.header.ram_size + info.header.bss_size;
    start = (u32)info.header.ram_address;
    end = start + total;
    for (r = gLoadedOverlays, i = 0; i < 12; r++, i++) {
        if (r->id == 0xffffffff) {
            if (slot == 0) {
                slot = r;
            }
        } else if (((r->start + r->size) > start) && (end > r->start)) {
            OS_Terminate();
        }
    }

    if (slot != 0) {
        slot->id = id;
        slot->start = start;
        slot->size = total;
    }
    FS_LoadOverlay(MI_PROCESSOR_ARM9, id);
    func_0203d7b8();
}

/* Poison an unloaded overlay's RAM (UnloadOverlay): invalidate both caches
   over its code and bss, then fill them with 0xe6000010, an ARM undefined
   instruction, so a stale call into it traps. */
void func_02017fd0(FSOverlayInfo *info) {
    u32 ramSize;
    u32 bssSize;
    void *ptr;
    u32 len;
    volatile u32 fillVal;

    ramSize = info->header.ram_size;
    bssSize = info->header.bss_size;
    ptr = info->header.ram_address;
    len = ramSize + bssSize;

    IC_InvalidateRange(ptr, len);
    DC_InvalidateRange(ptr, len);

    fillVal = 0xe6000010;
    MI_CpuFill32(ptr, fillVal, len);
}

void UnloadOverlay(int id) {
    FSOverlayInfo info;
    struct OverlaySlot *found = 0;
    unsigned int i;
    struct OverlaySlot *r = gLoadedOverlays;
    for (i = 0; i < 12; i++, r++) {
        if (r->id == id) {
            r->id = -1;
            r->start = 0;
            found = r;
            r->size = 0;
        }
    }
    if (found == 0)
        OS_Terminate();
    FS_UnloadOverlay(MI_PROCESSOR_ARM9, id);
    func_02018c00(&info, 0, id);
    if (func_0203d7b8() != 0)
        return;
    func_02017fd0(&info);
}

/* UnloadOverlay for an overlay that may not be loaded: free its slot, and if
   it had one, unload ARM9 overlay `key` and, unless func_0203d7b8 says
   otherwise, poison its RAM (func_02017fd0). Where UnloadOverlay crashes on
   an overlay with no slot, this returns. */
void func_02017e94(int key) {
    FSOverlayInfo info;
    struct OverlaySlot *found = 0;
    unsigned int i;
    for (i = 0; i < 12; i++) {
        struct OverlaySlot *e = &gLoadedOverlays[i];
        if (e->id == key) {
            e->id = -1;
            e->start = 0;
            found = e;
            e->size = 0;
        }
    }
    if (found == 0)
        return;
    FS_UnloadOverlay(MI_PROCESSOR_ARM9, key);
    func_02018c00(&info, 0, key);
    if (func_0203d7b8() != 0)
        return;
    func_02017fd0(&info);
}

/* Free every loaded-overlay slot. */
void func_02017e60(void) {
    unsigned int i;
    struct OverlaySlot *p = gLoadedOverlays;
    for (i = 0; i < 12; i++, p++) {
        p->id = -1;
        p->start = 0;
        p->size = 0;
    }
}
}
