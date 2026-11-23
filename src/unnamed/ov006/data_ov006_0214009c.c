#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[8];
} sSnowballBgNclPath_Layout;
typedef char sSnowballBgNclPath_size[sizeof(sSnowballBgNclPath_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} sSnowballNcgPath_Layout;
typedef char sSnowballNcgPath_size[sizeof(sSnowballNcgPath_Layout) == 0x20 ? 1 : -1];

typedef struct {
    u32 w[8];
} sSnowballNclPath_Layout;
typedef char sSnowballNclPath_size[sizeof(sSnowballNclPath_Layout) == 0x20 ? 1 : -1];

extern sSnowballBgNclPath_Layout sSnowballBgNclPath;
extern sSnowballNcgPath_Layout sSnowballNcgPath;
extern sSnowballNclPath_Layout sSnowballNclPath;

sSnowballNcgPath_Layout sSnowballNcgPath = {{
    0x2f474d2f,
    0x64325f64,
    0x5f676d5f,
    0x696b7579,
    0x616d6164,
    0x67636e5f,
    0x6e69622e,
    0x00000000,
}};

sSnowballBgNclPath_Layout sSnowballBgNclPath = {{
    0x2f474d2f,
    0x64325f64,
    0x5f676d5f,
    0x795f6762,
    0x64696b75,
    0x5f616d61,
    0x2e6c636e,
    0x006e6962,
}};

sSnowballNclPath_Layout sSnowballNclPath = {{
    0x2f474d2f,
    0x64325f64,
    0x5f676d5f,
    0x696b7579,
    0x616d6164,
    0x6c636e5f,
    0x6e69622e,
    0x00000000,
}};
