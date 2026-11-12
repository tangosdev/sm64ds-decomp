//cpp
/* dExtAnmTexPat_c::UpdateFileOffsets -- the BTP load-time fixup, arm9
 * 0x020468a0..0x020469ac. Every record table is stored file-relative; this
 * walks the header in order, rebasing each table pointer and the name word
 * inside each record (the same words func_02046d50 later reads through
 * Prepare).
 *
 * The records are walked through the shared views of their layout. The old
 * `{ int v = *(int *)(p + 4); if (v) ... = v + base; }` spelling was kept
 * for a CSE problem that a plain typed name-pointer assignment no longer
 * shows: through typed record pointers it builds the same bytes. The three
 * key-table pointers are now named from func_02046bbc's indexed reads.
 */
#include "dExtAnmTexPat_c.h"

#define REBASE(type, p) ((type *)((char *)(p) + base))

void dExtAnmTexPat_c::UpdateFileOffsets(BTP_File &file) {
#ifndef SM64DS_HOST_64
    int base = (int)&file;
#else
    /* The sum is stored back into a 4-byte slot, so the file has to sit in the low 4GB. */
    uintptr_t base = (uintptr_t)&file;
#endif
    int i = 0;
    if (file.texRecords)
        file.texRecords = REBASE(BTPNameRecord, file.texRecords);
    {
        BTPNameRecord *p = (BTPNameRecord *)file.texRecords;
        while (i < (int)file.numTexRecords) {
            if (p->name)
                p->name = REBASE(char, p->name);
            i++;
            p++;
        }
    }
    if (file.palRecords)
        file.palRecords = REBASE(BTPNameRecord, file.palRecords);
    {
        BTPNameRecord *q = (BTPNameRecord *)file.palRecords;
        int j = 0;
        while (j < (int)file.numPalRecords) {
            if (q->name)
                q->name = REBASE(char, q->name);
            j++;
            q++;
        }
    }
    if (file.keyFrames)
        file.keyFrames = REBASE(u16, file.keyFrames);
    if (file.keyTexIndices)
        file.keyTexIndices = REBASE(u16, file.keyTexIndices);
    if (file.keyPalIndices)
        file.keyPalIndices = REBASE(u16, file.keyPalIndices);
    if (file.seqRecords)
        file.seqRecords = REBASE(BTPSeqRecord, file.seqRecords);
    {
        BTPSeqRecord *r = (BTPSeqRecord *)file.seqRecords;
        int k = 0;
        while (k < (int)file.numSeqRecords) {
            if (r->name)
                r->name = REBASE(char, r->name);
            k++;
            r++;
        }
    }
}
