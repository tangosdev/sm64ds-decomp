#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov039_0211150c_Layout;
typedef char data_ov039_0211150c_size[sizeof(data_ov039_0211150c_Layout) == 0xc ? 1 : -1];

extern data_ov039_0211150c_Layout data_ov039_0211150c;

data_ov039_0211150c_Layout data_ov039_0211150c = {{
    0xffffffff,
    0xffffffff,
    0xffffffff,
}};
