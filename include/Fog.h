/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Fog: 1 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FOG_H
#define FOG_H
#include "types.h"

struct Fog {
    u8  pad_000[0x20];
    u8  unk_020;            /* 0x020 */
    u8  unk_021;            /* 0x021 */
    u16 unk_022;            /* 0x022 */
    u16 unk_024;            /* 0x024 */
};

/* Over-determined, not assumed: Fog's own fields end at 0x026 and its widest
   member is u16, so sizeof rounds to 0x26; independently, Stage.h places Fog at
   0x96c with its next real field at 0x994 behind pad_992[0x2], i.e. a 0x26 span.
   check_header_offsets.py sizes a member type from this typedef -- without it,
   `Fog unk_96c;` is UNPARSED and blinds the checker to the rest of Stage.h. */
typedef char Fog_size_must_be_0x26[sizeof(struct Fog) == 0x26 ? 1 : -1];
typedef struct Fog Fog;

#endif
