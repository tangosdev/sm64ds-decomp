//cpp
// @symbol _ZN15TextureSequence17UpdateFileOffsetsER8BTP_File
#include "TextureSequence.h"

#define REBASE(p) ((char *)(p) + base)

void TextureSequence::UpdateFileOffsets(BTP_File &file)
{
    int base = (int)&file;
    int i = 0;
    if (file.unk_04) file.unk_04 = REBASE(file.unk_04);
    {
        char *p = file.unk_04;
        while (i < (int)file.numTexRecords) {
            if (*(int *)(p + 4)) *(int *)(p + 4) += base;
            i++;
            p += 8;
        }
    }
    if (file.unk_0c) file.unk_0c = REBASE(file.unk_0c);
    {
        char *q = file.unk_0c;
        int j = 0;
        while (j < (int)file.numPalRecords) {
            if (*(int *)(q + 4)) *(int *)(q + 4) += base;
            j++;
            q += 8;
        }
    }
    if (file.unk_10) file.unk_10 = REBASE(file.unk_10);
    if (file.unk_14) file.unk_14 = REBASE(file.unk_14);
    if (file.unk_18) file.unk_18 = REBASE(file.unk_18);
    if (file.unk_20) file.unk_20 = REBASE(file.unk_20);
    {
        char *r = file.unk_20;
        int k = 0;
        while (k < (int)file.numSeqRecords) {
            if (*(int *)(r + 4)) *(int *)(r + 4) += base;
            k++;
            r += 0xc;
        }
    }
}
