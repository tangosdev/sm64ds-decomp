#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[1];
} data_ov030_021159f4_Layout;
typedef char data_ov030_021159f4_size[sizeof(data_ov030_021159f4_Layout) == 0x4 ? 1 : -1];

extern data_ov030_021159f4_Layout data_ov030_021159f4;

data_ov030_021159f4_Layout data_ov030_021159f4 = {{
    0x00412000,
}};
