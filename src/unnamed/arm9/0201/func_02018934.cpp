//cpp
#include "types.h"
#include "nitro/os/cache.h"
#include "Heap.h"
#include "nitro/fs/file.h"

extern "C" {

extern int myFS_OpenFile(FSFile *p_file, const char *path);
extern int myFS_ReadFile(FSFile *p_file, void *dst, int len);
extern int func_0204ee40(void *archive, void *name, void *narc);

/* The mounted-archive head func_0204ee40 fills. The file image starts after
   it; the trailing word leaves the image 16-byte aligned. */
struct MountedNarc {
    FSArchive arc;
    void *narc;
    void *fatb;
    void *fimg;
    u32 reserved;
};

typedef char MountedNarc_size_must_be_0x60[sizeof(struct MountedNarc) == 0x60 ? 1 : -1];

/* LoadArchive's loader: read the .narc at path whole into one heap block,
   after a 0x60-byte head, and mount it there under name (func_0204ee40).
   Returns the block, or 0 (freeing it) if the read or the mount fails. */
void *func_02018934(char *name, char *path, void *heap) {
    FSFile file;
    void *result = 0;
    struct MountedNarc *archive;
    void *narc;
    int diff, aligned;

    FS_InitFile(&file);
    myFS_OpenFile(&file, path);
    diff = FS_GetLength(&file);
    aligned = (diff + 15) & ~15;
    archive =
        (struct MountedNarc *)((mHeap::Heap_t *)heap)->_Allocate(aligned + sizeof(*archive), -0x10);
    narc = archive + 1;
    DC_FlushRange(narc, aligned);
    if (myFS_ReadFile(&file, narc, diff) == diff && func_0204ee40(archive, name, narc)) {
        result = archive;
    } else {
        ((mHeap::Heap_t *)heap)->_Deallocate(archive);
    }
    FS_CloseFile(&file);
    return result;
}
}
