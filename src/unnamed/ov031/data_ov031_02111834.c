#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[4];
} data_ov031_02111834_Layout;
typedef char data_ov031_02111834_size[sizeof(data_ov031_02111834_Layout) == 0x10 ? 1 : -1];

typedef struct {
    u32 w[41];
} data_ov031_02111844_Layout;
typedef char data_ov031_02111844_size[sizeof(data_ov031_02111844_Layout) == 0xa4 ? 1 : -1];

extern data_ov031_02111834_Layout data_ov031_02111834;
extern data_ov031_02111844_Layout data_ov031_02111844;

data_ov031_02111834_Layout data_ov031_02111834 = {{
    0xf59cf4ef,
    0xe524e225,
    0xf632f4b2,
    0xe524e225,
}};

data_ov031_02111844_Layout data_ov031_02111844 = {{
    0xf6c8f0b0, 0xe6b4e225, 0x0379eec8, 0xe844e225, 0x0667ee62, 0xea6ae225, 0x088dedfe,
    0xf87ae225, 0x0a4feba9, 0xfad2e225, 0x08bfeadf, 0xfbcce225, 0x0667ea33, 0xfc94e225,
    0xfd08e953, 0x0f21e225, 0xeef8e2d8, 0x1a43e225, 0xef2adf08, 0x2371e225, 0xefc0dcf4,
    0x2d03e225, 0xedfedb85, 0x2f5be225, 0xeb74dada, 0x2f5be225, 0xe1b0d93e, 0x2e2fe225,
    0xd882d7e0, 0x2a79e225, 0xd4ccd718, 0x1f57e225, 0xd274d566, 0x1ec1e225, 0xd274d546,
    0x1e2be225, 0xd274d525, 0x1d95e225, 0xd274d505, 0x1cffe225, 0xd274d4e4,
}};
