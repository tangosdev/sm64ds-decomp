/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class PathPtr: 5 matched functions, 2 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef PATHPTR_H
#define PATHPTR_H
#include "types.h"

/* fwd */
struct Vector3;
struct idx;
struct vec_;
struct PathPtr {
    s32 unk_000;            /* 0x000 */
    s32 unk_004;            /* 0x004 */
#ifdef __cplusplus
    /* methods */
    unsigned int Loops() const;
    unsigned int NumNodes() const;
    void GetNode(Vector3 & vec_, unsigned int idx) const;
#endif
};

/* Two words and nothing else, corroborated from outside the class:
 * MetalNetLift_Spawn allocates 0x368 bytes and constructs a PathPtr at
 * +0x360, so 8 is exactly what closes that object. Asserting it also lets
 * check_header_offsets size a PathPtr member instead of skipping the line. */
typedef char PathPtr_size_must_be_0x8[sizeof(struct PathPtr) == 0x8 ? 1 : -1];

#endif
