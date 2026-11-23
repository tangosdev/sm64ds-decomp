#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[3];
} data_ov058_02111754_Layout;
typedef char data_ov058_02111754_size[sizeof(data_ov058_02111754_Layout) == 0xc ? 1 : -1];

extern data_ov058_02111754_Layout data_ov058_02111754;

data_ov058_02111754_Layout data_ov058_02111754 = {{
    0x025811e9,
    0x130ef049,
    0xedb50258,
}};
