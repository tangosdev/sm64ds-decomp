#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov019_02112c38_Layout;
typedef char data_ov019_02112c38_size[sizeof(data_ov019_02112c38_Layout) == 0xc ? 1 : -1];

extern data_ov019_02112c38_Layout data_ov019_02112c38;

data_ov019_02112c38_Layout data_ov019_02112c38 = {{
    0xeb4c00a0,
    0xe8901964,
    0xc0000000,
}};
