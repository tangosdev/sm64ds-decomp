//cpp
#include "AsyncJob.h"
#include "Memory.h"
#include "Sound.h"
#include "private/fs_views.h"

extern "C" {

/* func_0201a2f8 at 0x0201a2f8
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (arm9 main).
 */
extern void func_0201fec8(void);
extern void func_02018aa4(void);
extern void func_020134c8(void);
extern int GetSoundMode(void);
extern void SetSoundMode(int mode);
extern u32 FS_TryLoadTable(void *p_mem, u32 size);
extern void LoadOverlay(int id);
extern void InitializeAssetHandleTable(void);
extern void UnloadOverlay(int id);
extern FSArchive *FS_FindArchive(const char *name, int name_len);
extern void *FS_UnloadArchiveTables(void *p_arc);
extern void LoadFont(int id);

extern int OVERLAY_0_ID;
extern int OVERLAY_1_ID;
extern char data_0208ee5c[];

void func_0201a2f8(int unused) {
    int cond = (*(unsigned short*)0x27ffc40 == 2);
    void* ptr;
    if (cond) {
        func_0201fec8();
        func_02018aa4();
        func_020134c8();
        SetSoundMode(GetSoundMode());
    }
    ptr = 0;
    if (!cond) {
        int size = FS_TryLoadTable(0, 0);
        ptr = Memory::Allocate(size);
        FS_TryLoadTable(ptr, size);
    }
    LoadOverlay((int)&OVERLAY_0_ID);
    InitializeAssetHandleTable();
    UnloadOverlay((int)&OVERLAY_0_ID);
    if (ptr != 0) {
        FS_UnloadArchiveTables(FS_FindArchive(data_0208ee5c, 3));
        Memory::Deallocate(ptr);
    }
    LoadOverlay((int)&OVERLAY_1_ID);
    if (cond) {
        Sound::LoadGroupAndSetBank(0x2f, 0x36);
    } else {
        Sound::LoadInitialGroup(0);
    }
    if (cond) return;
    LoadFont(0);
}
}
