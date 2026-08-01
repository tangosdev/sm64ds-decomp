/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Message: 15 matched functions, 1 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef MESSAGE_H
#define MESSAGE_H
#include "types.h"

/* fwd */
struct msg;
struct Message {
    u8  pad_000[0x196];
    u8  unk_196;            /* 0x196 */
#ifdef __cplusplus
    /* methods */
    void Display(unsigned int msg);
#endif
};

#endif
