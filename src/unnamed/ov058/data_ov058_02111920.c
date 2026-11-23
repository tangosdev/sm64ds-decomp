#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[2];
} data_ov058_02111920_Layout;
typedef char data_ov058_02111920_size[sizeof(data_ov058_02111920_Layout) == 0x8 ? 1 : -1];

extern data_ov058_02111920_Layout data_ov058_02111920;

data_ov058_02111920_Layout data_ov058_02111920 = {{
    0x53504c43,
    0x000f0008,
}};
