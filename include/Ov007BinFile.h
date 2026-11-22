#ifndef OV007_BIN_FILE_H
#define OV007_BIN_FILE_H

/* A reconstructed layout and name, not recovered class identity. */

#ifdef __cplusplus
extern "C" {
#endif

/* The common Nitro binary file header (field names as NitroSystem's
 * NNSG2dBinaryFileHeader); only the block count is read. The blocks start
 * 0x10 bytes in, the header's usual size. */
typedef struct Ov007BinFileHeader {
    char pad_00[0xe];
    unsigned short dataBlocks; /* 0x0e */
} Ov007BinFileHeader;

/* Each block's 8-byte header (NNSG2dBinaryBlockHeader's names). */
typedef struct Ov007BinBlockHeader {
    unsigned int kind; /* 0x00: the block's four-character code */
    unsigned int size; /* 0x04: header included; the next block starts here */
} Ov007BinBlockHeader;

/* The 0x84-byte block index func_ov007_020c9a2c fills on its caller's
 * stack. func_ov007_020c95fc, func_ov007_020c9688, func_ov007_020c9988 and
 * func_ov007_020c99d8 read blockData[0]; func_ov007_020c9688 also reads
 * [3]. */
typedef struct Ov007BinFile {
    Ov007BinFileHeader *header;      /* 0x00 */
    Ov007BinBlockHeader *blocks[16]; /* 0x04 */
    void *blockData[16];             /* 0x44: each block's data, past its header */
} Ov007BinFile;

typedef char Ov007BinFile_size_must_be_0x84[sizeof(Ov007BinFile) == 0x84 ? 1 : -1];

/* Indexes the blocks of the file at address `file`, passed as an int: either
 * an entry of one of ov007's int tables (func_ov007_020b6d40 reads
 * data_ov007_020d7e28 and its neighbours) or a cached file pointer cast to
 * int (func_ov007_020bcdb8, from func_ov007_020b3f20). */
void func_ov007_020c9a2c(Ov007BinFile *f, int file);

#ifdef __cplusplus
}
#endif

#endif
