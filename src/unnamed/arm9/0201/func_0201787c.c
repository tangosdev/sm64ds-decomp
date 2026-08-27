#include "types.h"
/* func_0201787c at 0x0201787c
 * SharedFilePtr LoadFile variant: loads file, if first ref and non-null, calls Model::UpdateFileOffsets.
 * Returns the raw file pointer.
 */
struct SharedFilePtr {
    unsigned short fileID;  /* 0x00 */
    u8 numRefs;             /* 0x02 */
    char pad;               /* 0x03 */
    void *filePtr;          /* 0x04 */
};

struct BMD_File;

extern void _ZN13SharedFilePtr8LoadFileEv(struct SharedFilePtr *sfp);
extern void _ZN5Model17UpdateFileOffsetsER8BMD_File(struct BMD_File *file);

void *func_0201787c(struct SharedFilePtr *sfp)
{
    void *filePtr;
    _ZN13SharedFilePtr8LoadFileEv(sfp);
    filePtr = sfp->filePtr;
    if (sfp->numRefs == 1 && filePtr != 0)
    {
        _ZN5Model17UpdateFileOffsetsER8BMD_File((struct BMD_File *)filePtr);
    }
    return filePtr;
}
