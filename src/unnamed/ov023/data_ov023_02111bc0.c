#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 f000;
    void *f004;
    u32 f008;
    void *f00c;
    u32 f010;
} data_ov023_02111bc0_Layout;
typedef char data_ov023_02111bc0_size[sizeof(data_ov023_02111bc0_Layout) == 0x14 ? 1 : -1];

extern data_ov023_02111bc0_Layout data_ov023_02111bc0;
extern u8 data_ov023_02111c98[];
extern u8 data_ov023_02111b54[];

data_ov023_02111bc0_Layout data_ov023_02111bc0 = {
    0x00001902, &data_ov023_02111c98[0], 0x00000203, &data_ov023_02111b54[0], 0x00000204,
};
