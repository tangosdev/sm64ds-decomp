//cpp
#include "AsyncJob.h"
#include "Memory.h"
#include "Sound.h"
#include "nitro/fs/archive.h"
#include "nitro/mb/mb.h"

extern void func_0201fec8(void);
extern "C" {

extern void func_02018aa4(void);
}

extern void func_020134c8(void);
extern "C" {

extern int GetSoundMode(void);
extern void SetSoundMode(int mode);
}

extern "C" u32 FS_TryLoadTable(void *p_mem, u32 size);
extern "C" {

extern void LoadOverlay(int id);
extern void InitializeAssetHandleTable(void);
extern void UnloadOverlay(int id);
}

extern "C" void LoadFont(unsigned char id);

extern int OVERLAY_0_ID;
extern int OVERLAY_1_ID;
extern char data_0208ee5c[];

/* The boot job (dScBoot_c and the download-play scene start it through
 * func_0201a244). A download-play child first runs the start-up steps
 * func_0201a054 left out: the link start (func_0201fec8), the file system
 * (func_02018aa4) and func_020134c8, then reapplies the sound mode. A card
 * boot loads the FS table into a heap block for the duration. Overlay 0
 * builds the asset handle table and is unloaded again, and overlay 1 stays.
 * A child loads sound group 0x2f with bank 0x36 and no font; a card boot
 * loads the initial sound group and font 0. */
extern "C" void func_0201a2f8(int unused) {
    int isChild = MB_IsMultiBootChild();
    void *ptr;
    if (isChild) {
        func_0201fec8();
        func_02018aa4();
        func_020134c8();
        SetSoundMode(GetSoundMode());
    }
    ptr = 0;
    if (!isChild) {
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
    if (isChild) {
        Sound::LoadGroupAndSetBank(0x2f, 0x36);
    } else {
        Sound::LoadInitialGroup(0);
    }
    if (!isChild) {
        LoadFont(0);
    }
}
