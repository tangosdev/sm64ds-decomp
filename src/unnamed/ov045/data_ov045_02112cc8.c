#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov045_02112cc8_Layout;
typedef char data_ov045_02112cc8_size[sizeof(data_ov045_02112cc8_Layout) == 0x8 ? 1 : -1];

extern data_ov045_02112cc8_Layout data_ov045_02112cc8;

data_ov045_02112cc8_Layout data_ov045_02112cc8 = {{
    0x75726167,
    0x0000635f,
}};
