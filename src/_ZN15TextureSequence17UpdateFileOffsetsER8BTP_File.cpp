//cpp
// @symbol _ZN15TextureSequence17UpdateFileOffsetsER8BTP_File
/* TextureSequence::UpdateFileOffsets -- the BTP load-time fixup, arm9
 * 0x020468a0..0x020469ac. Every record table is stored file-relative; this
 * walks the header in order, rebasing each table pointer and the name word
 * inside each record (the same words func_02046d50 later reads through
 * Prepare).
 *
 * leftovers: each record loop keeps the `{ int v = *(int *)(p + 4);
 * if (v) *(int *)(p + 4) = v + base; }` spelling -- written as a normal
 * load/add/store mwcc CSEs the p+4 address into its own register and
 * reloads through it (addne r2,r3,#4 / ldrne r1,[r2]), two extra words per
 * loop. unk_10/unk_14/unk_18 stay placeholders: UpdateFileOffsets rebases
 * them but neither it nor func_02046d50 reads what they point at.
 */
#include "TextureSequence.h"

#define REBASE(p) ((char *)(p) + base)

void TextureSequence::UpdateFileOffsets(BTP_File &file)
{
    int base = (int)&file;
    int i = 0;
    if (file.texRecords) file.texRecords = REBASE(file.texRecords);
    {
        char *p = file.texRecords;
        while (i < (int)file.numTexRecords) {
            { int v = *(int *)(p + 4); if (v) *(int *)(p + 4) = v + base; }
            i++;
            p += 8;
        }
    }
    if (file.palRecords) file.palRecords = REBASE(file.palRecords);
    {
        char *q = file.palRecords;
        int j = 0;
        while (j < (int)file.numPalRecords) {
            { int v = *(int *)(q + 4); if (v) *(int *)(q + 4) = v + base; }
            j++;
            q += 8;
        }
    }
    if (file.unk_10) file.unk_10 = REBASE(file.unk_10);
    if (file.unk_14) file.unk_14 = REBASE(file.unk_14);
    if (file.unk_18) file.unk_18 = REBASE(file.unk_18);
    if (file.seqRecords) file.seqRecords = REBASE(file.seqRecords);
    {
        char *r = file.seqRecords;
        int k = 0;
        while (k < (int)file.numSeqRecords) {
            { int v = *(int *)(r + 4); if (v) *(int *)(r + 4) = v + base; }
            k++;
            r += 0xc;
        }
    }
}
