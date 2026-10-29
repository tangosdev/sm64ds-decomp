typedef enum { FALSE, TRUE } Bool;

/* The 0x84-byte block index func_ov007_020c9a2c fills; names as there,
 * untyped here: only block 0's data is read. */
typedef struct Ov007BinFile {
    void *header;        /* 0x00 */
    void *blocks[16];    /* 0x04 */
    void *blockData[16]; /* 0x44 */
} Ov007BinFile;

/* Block 0 of a character file: a 0xc-byte header, then the tile data. Each
 * slot is read as a byte; func_ov007_020c3550 keeps them in the record
 * func_ov007_020c33d0 loads into VRAM. */
typedef struct Ov007CharBlock {
    unsigned char width; /* 0x00: in tiles (the record's +8) */
    char pad_01[3];
    unsigned char height; /* 0x04: in tiles (the record's +9) */
    char pad_05[3];
    /* 0x08: func_ov007_020c33d0 sizes each tile 0x40 bytes (8bpp) when set,
     * else 0x20 (4bpp) */
    unsigned char is8bpp;
    char pad_09[3];
} Ov007CharBlock;

typedef char Ov007BinFile_size_must_be_0x84[sizeof(Ov007BinFile) == 0x84 ? 1 : -1];
typedef char Ov007CharBlock_size_must_be_0xc[sizeof(Ov007CharBlock) == 0xc ? 1 : -1];

extern void func_ov007_020c9a2c(void *buf, int x);
extern void func_ov007_020c3550(int a, int b, int c, int d);

/* Builds the character record for file x: its tiles follow block 0's header. */
int func_ov007_020c99d8(int x)
{
    Ov007BinFile buf;
    Ov007CharBlock *ip;
    unsigned char a;
    unsigned char b;
    unsigned char c;
    Bool d;

    if (x == 0) {
        return 0;
    }
    func_ov007_020c9a2c(&buf, x);
    ip = (Ov007CharBlock *)buf.blockData[0];
    a = ip->is8bpp;
    b = ip->width;
    c = ip->height;
    if (a == 0) {
        d = FALSE;
    } else {
        d = TRUE;
    }
    func_ov007_020c3550((int)(ip + 1), b, c, d);
}
