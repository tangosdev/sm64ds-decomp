/* The common Nitro binary file header (field names as NitroSystem's
 * NNSG2dBinaryFileHeader); only the block count is read here. The blocks
 * start 0x10 bytes in, the header's usual size. */
typedef struct Ov007BinFileHeader {
    char pad_00[0xe];
    unsigned short dataBlocks; /* 0x0e */
} Ov007BinFileHeader;

/* Each block's 8-byte header (NNSG2dBinaryBlockHeader's names). */
typedef struct Ov007BinBlockHeader {
    unsigned int kind; /* 0x00: the block's four-character code */
    unsigned int size; /* 0x04: header included; the next block starts here */
} Ov007BinBlockHeader;

/* The 0x84-byte index this function fills for its callers
 * (func_ov007_020c95fc, func_ov007_020c9688, func_ov007_020c9988 and
 * func_ov007_020c99d8 read blockData[0], func_ov007_020c9688 also [3]). */
typedef struct Ov007BinFile {
    Ov007BinFileHeader *header;      /* 0x00 */
    Ov007BinBlockHeader *blocks[16]; /* 0x04 */
    void *blockData[16];             /* 0x44: each block's data, past its header */
} Ov007BinFile;

typedef char Ov007BinFile_size_must_be_0x84[sizeof(Ov007BinFile) == 0x84 ? 1 : -1];

/* The parameter keeps the tree's int * spelling; `f` is the typed view. */
#pragma opt_strength_reduction off
void func_ov007_020c9a2c(int *o, int x) {
    Ov007BinFile *f = (Ov007BinFile *)o;
    Ov007BinFileHeader *base;
    int i;

    f->header = (Ov007BinFileHeader *)x;
    base = f->header;
    x += 0x10;
    for (i = 0; i < base->dataBlocks; i++) {
        Ov007BinBlockHeader *q;
        f->blocks[i] = (Ov007BinBlockHeader *)x;
        q = f->blocks[i];
        f->blockData[i] = (void *)(x + 8);
        x += q->size;
    }
}
