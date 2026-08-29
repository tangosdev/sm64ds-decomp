#ifndef CLPS_BLOCK_REF_H
#define CLPS_BLOCK_REF_H

struct CLPS_Block;

/* One-word value handle held by dBgW_Kc. Its constructor, assignment and
 * destructor shapes are all ROM-proven at 0x02038228, 0x0203821c and
 * 0x02038224. The exact original wrapper spelling is not. */
struct CLPS_BlockRef {
    CLPS_Block *ptr;

    CLPS_BlockRef();
    ~CLPS_BlockRef();
    CLPS_BlockRef &operator=(CLPS_Block &block);
};

typedef char CLPS_BlockRef_size_must_be_0x4[
    sizeof(CLPS_BlockRef) == 0x4 ? 1 : -1];

#endif /* CLPS_BLOCK_REF_H */
