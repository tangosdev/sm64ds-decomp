//cpp
// @symbol _ZN13SharedFilePtr7ReleaseEv
/* SharedFilePtr::Release -- hands one reference back. Publishes the file ID to
 * the global at 0x0209d3bc (every entry point here does, so a fault mid-load
 * names the file), then drops the refcount and lets go of the file when the
 * last holder releases it.
 *
 * THE FIELDS STAY RAW OFFSETS, and that is include/SharedFilePtr.h's standing
 * decision, not an omission: the layout is not recovered, and the files that
 * declare this struct locally disagree about it -- {int a, file;}, {int
 * data[4];}, {int f0; void *f4;} and {int h;} all appear, each sized to
 * whatever arithmetic that file needed. Committing to one in the header would
 * silently change the others' indexing. What this file does buy is the
 * DEFINITION: as a real method the compiler produces _ZN13SharedFilePtr7ReleaseEv
 * itself, instead of a C file emitting that identifier verbatim -- the spelling
 * that a C++ caller mangles a second time into a symbol nothing defines.
 *
 *   +0x0  u16  fileID
 *   +0x2  u8   numRefs
 *   +0x4  void *filePtr   (not touched here) */
#include "SharedFilePtr.h"

extern "C" {
extern unsigned int data_0209d3bc;
void func_02017c24(SharedFilePtr *self);
}

void SharedFilePtr::Release()
{
    char *self = (char *)this;

    data_0209d3bc = *(unsigned short *)self;

    if (*(unsigned char *)(self + 2) == 0)
        return;

    *(unsigned char *)(self + 2) -= 1;

    if (*(unsigned char *)(self + 2) != 0)
        return;

    func_02017c24(this);
}
