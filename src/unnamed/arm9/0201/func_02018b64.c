#include "nitro/fs/overlay.h"
#include "private/fs_views.h"
/* NitroSDK FS_LoadOverlayImage: open the overlay's file by its file_id,
 * clear the overlay's RAM image (FS_ClearOverlayImage), read
 * the whole file to ram_address (myFS_ReadFile), then pass the target
 * processor, RAM address, ROM offset and RAM size to func_0206d32c. */
typedef struct FSFile FSFile;

extern void FS_InitFile(void *s);
extern FSFileID FS_GetOverlayFileID(const FSOverlayInfo *p_ovi);
extern int myFS_OpenFileFast(void *a, FSFileID bc);
extern void FS_ClearOverlayImage(FSOverlayInfo *p_ovi);
extern int myFS_ReadFile(void *a, int b, int c);
extern int func_0206d32c(int target, unsigned int ramAddress, unsigned int romOffset,
                         unsigned int ramSize);
extern int FS_CloseFile(void *self);

int func_02018b64(FSOverlayInfo *ovi) {
    FSFile file;
    FSFileID fileId;

    FS_InitFile(&file);
    fileId = FS_GetOverlayFileID(ovi);
    if (myFS_OpenFileFast(&file, fileId) == 0)
        return 0;

    FS_ClearOverlayImage(ovi);
    myFS_ReadFile(&file, (int)ovi->header.ram_address, file.prop.file.bottom - file.prop.file.top);

    {
        int ramSize = ovi->header.ram_size;
        int romOffset = file.prop.file.top;
        int ramAddress = (int)ovi->header.ram_address;
        /* the plain `ovi->target != 0` changes the .text bytes */
        int flag = (int)((ovi->target == 0) ? (char *)0 : (char *)1);
        func_0206d32c(flag, ramAddress, romOffset, ramSize);
    }

    FS_CloseFile(&file);
    return 1;
}
