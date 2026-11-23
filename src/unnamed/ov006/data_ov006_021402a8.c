#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[7];
} sFlowerNcgPath_Layout;
typedef char sFlowerNcgPath_size[sizeof(sFlowerNcgPath_Layout) == 0x1c ? 1 : -1];

typedef struct {
    u32 w[7];
} sFlowerNclPath_Layout;
typedef char sFlowerNclPath_size[sizeof(sFlowerNclPath_Layout) == 0x1c ? 1 : -1];

extern sFlowerNcgPath_Layout sFlowerNcgPath;
extern sFlowerNclPath_Layout sFlowerNclPath;

sFlowerNcgPath_Layout sFlowerNcgPath = {{
    0x2f474d2f,
    0x64325f64,
    0x5f676d5f,
    0x776f6c66,
    0x6e5f7265,
    0x622e6763,
    0x00006e69,
}};

sFlowerNclPath_Layout sFlowerNclPath = {{
    0x2f474d2f,
    0x64325f64,
    0x5f676d5f,
    0x776f6c66,
    0x6e5f7265,
    0x622e6c63,
    0x00006e69,
}};
