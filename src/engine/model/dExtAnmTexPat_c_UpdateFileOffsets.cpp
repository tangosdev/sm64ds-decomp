//cpp
// @symbol _ZN15dExtAnmTexPat_c17UpdateFileOffsetsER8BTP_File
/* dExtAnmTexPat_c::UpdateFileOffsets -- the BTP load-time fixup, arm9
 * 0x020468a0..0x020469ac. Every record table is stored file-relative; this
 * walks the header in order, rebasing each table pointer and the name word
 * inside each record (the same words func_02046d50 later reads through
 * Prepare).
 *
 * The records are walked through file-local views of their layout. The old
 * `{ int v = *(int *)(p + 4); if (v) ... = v + base; }` spelling was kept
 * for a CSE problem that the plain `if (p->name) p->name = REBASE(p->name)`
 * no longer shows: through typed record pointers it builds the same bytes.
 * unk_10/unk_14/unk_18 stay placeholders: UpdateFileOffsets rebases them
 * but neither it nor func_02046d50 reads what they point at.
 */
#include "dExtAnmTexPat_c.h"

#define REBASE(p) ((char *)(p) + base)

/* A texRecords/palRecords entry: func_02046d50 resolves name against the
   BMD's texture (func_020471ac) or palette (func_02047144) table and stores
   the index in resourceId. */
struct BTPNameRecord {
    u16 resourceId; /* 0x00 */
    u16 pad_02;
    char *name; /* 0x04 - rebased here */
};

/* A seqRecords entry: func_02046d50 resolves name against the BMD's
   material table (func_02046590) and stores the index at 0. */
struct BTPSeqRecord {
    u16 materialId; /* 0x00 */
    u16 pad_02;
    char *name; /* 0x04 - rebased here */
    u8 pad_08[4];
};

typedef char BTPNameRecord_size_must_be_8[sizeof(BTPNameRecord) == 8 ? 1 : -1];
typedef char BTPSeqRecord_size_must_be_0xc[sizeof(BTPSeqRecord) == 0xc ? 1 : -1];

void dExtAnmTexPat_c::UpdateFileOffsets(BTP_File &file) {
    int base = (int)&file;
    int i = 0;
    if (file.texRecords)
        file.texRecords = REBASE(file.texRecords);
    {
        BTPNameRecord *p = (BTPNameRecord *)file.texRecords;
        while (i < (int)file.numTexRecords) {
            if (p->name)
                p->name = REBASE(p->name);
            i++;
            p++;
        }
    }
    if (file.palRecords)
        file.palRecords = REBASE(file.palRecords);
    {
        BTPNameRecord *q = (BTPNameRecord *)file.palRecords;
        int j = 0;
        while (j < (int)file.numPalRecords) {
            if (q->name)
                q->name = REBASE(q->name);
            j++;
            q++;
        }
    }
    if (file.unk_10)
        file.unk_10 = REBASE(file.unk_10);
    if (file.unk_14)
        file.unk_14 = REBASE(file.unk_14);
    if (file.unk_18)
        file.unk_18 = REBASE(file.unk_18);
    if (file.seqRecords)
        file.seqRecords = REBASE(file.seqRecords);
    {
        BTPSeqRecord *r = (BTPSeqRecord *)file.seqRecords;
        int k = 0;
        while (k < (int)file.numSeqRecords) {
            if (r->name)
                r->name = REBASE(r->name);
            k++;
            r++;
        }
    }
}
