#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[4];
} ov033_02111c0c_Layout;
typedef char ov033_02111c0c_size[sizeof(ov033_02111c0c_Layout) == 0x10 ? 1 : -1];

#pragma define_section sName ".data.gap1"                                                          \
                             ".data.gap1"                                                          \
                             ".data.gap1" far_data
#pragma section sName begin

static ov033_02111c0c_Layout ov033_02111c0c = {{
    0xffffffff,
    0xffffffff,
    0xffffffff,
    0xffffffff,
}};
