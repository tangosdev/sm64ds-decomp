#include "Ov007BinFile.h"

void func_ov007_020c9a2c(Ov007BinFile *f, int file) {
    Ov007BinFileHeader *base;
    int i;

    f->header = (Ov007BinFileHeader *)file;
    base = f->header;
    file += 0x10;
    for (i = 0; i < base->dataBlocks; i++) {
        Ov007BinBlockHeader *q;
        f->blocks[i] = (Ov007BinBlockHeader *)file;
        q = f->blocks[i];
        f->blockData[i] = (void *)(file + 8);
        file += q->size;
    }
}
