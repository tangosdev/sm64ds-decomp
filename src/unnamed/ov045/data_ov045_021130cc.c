#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov045_021130cc_Layout;
typedef char data_ov045_021130cc_size[sizeof(data_ov045_021130cc_Layout) == 0x4 ? 1 : -1];

extern data_ov045_021130cc_Layout data_ov045_021130cc;

data_ov045_021130cc_Layout data_ov045_021130cc = {{
    0x00635f6b,
}};
