#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    u32 w[9];
} sFlowerBgUpNcgPath_Layout;
typedef char sFlowerBgUpNcgPath_size[sizeof(sFlowerBgUpNcgPath_Layout) == 0x24 ? 1 : -1];

typedef struct {
    u32 w[9];
} sFlowerBgDownNcgPath_Layout;
typedef char sFlowerBgDownNcgPath_size[sizeof(sFlowerBgDownNcgPath_Layout) == 0x24 ? 1 : -1];

typedef struct {
    u32 w[9];
} sFlowerBgUpNscPath_Layout;
typedef char sFlowerBgUpNscPath_size[sizeof(sFlowerBgUpNscPath_Layout) == 0x24 ? 1 : -1];

typedef struct {
    u32 w[9];
} sFlowerBgDownNscPath_Layout;
typedef char sFlowerBgDownNscPath_size[sizeof(sFlowerBgDownNscPath_Layout) == 0x24 ? 1 : -1];

typedef struct {
    u32 w[9];
} sFlowerBgUpNclPath_Layout;
typedef char sFlowerBgUpNclPath_size[sizeof(sFlowerBgUpNclPath_Layout) == 0x24 ? 1 : -1];

typedef struct {
    u32 w[9];
} sFlowerBgDownNclPath_Layout;
typedef char sFlowerBgDownNclPath_size[sizeof(sFlowerBgDownNclPath_Layout) == 0x24 ? 1 : -1];

extern sFlowerBgUpNcgPath_Layout sFlowerBgUpNcgPath;
extern sFlowerBgDownNcgPath_Layout sFlowerBgDownNcgPath;
extern sFlowerBgUpNscPath_Layout sFlowerBgUpNscPath;
extern sFlowerBgDownNscPath_Layout sFlowerBgDownNscPath;
extern sFlowerBgUpNclPath_Layout sFlowerBgUpNclPath;
extern sFlowerBgDownNclPath_Layout sFlowerBgDownNclPath;

sFlowerBgUpNclPath_Layout sFlowerBgUpNclPath = {{
    0x2f474d2f,
    0x64325f64,
    0x5f676d5f,
    0x665f6762,
    0x65776f6c,
    0x70755f72,
    0x6c636e5f,
    0x6e69622e,
    0x00000000,
}};

sFlowerBgDownNscPath_Layout sFlowerBgDownNscPath = {{
    0x2f474d2f,
    0x64325f64,
    0x5f676d5f,
    0x665f6762,
    0x65776f6c,
    0x6f645f72,
    0x6e5f6e77,
    0x622e6373,
    0x00006e69,
}};

sFlowerBgUpNscPath_Layout sFlowerBgUpNscPath = {{
    0x2f474d2f,
    0x64325f64,
    0x5f676d5f,
    0x665f6762,
    0x65776f6c,
    0x70755f72,
    0x63736e5f,
    0x6e69622e,
    0x00000000,
}};

sFlowerBgDownNcgPath_Layout sFlowerBgDownNcgPath = {{
    0x2f474d2f,
    0x64325f64,
    0x5f676d5f,
    0x665f6762,
    0x65776f6c,
    0x6f645f72,
    0x6e5f6e77,
    0x622e6763,
    0x00006e69,
}};

sFlowerBgUpNcgPath_Layout sFlowerBgUpNcgPath = {{
    0x2f474d2f,
    0x64325f64,
    0x5f676d5f,
    0x665f6762,
    0x65776f6c,
    0x70755f72,
    0x67636e5f,
    0x6e69622e,
    0x00000000,
}};

sFlowerBgDownNclPath_Layout sFlowerBgDownNclPath = {{
    0x2f474d2f,
    0x64325f64,
    0x5f676d5f,
    0x665f6762,
    0x65776f6c,
    0x6f645f72,
    0x6e5f6e77,
    0x622e6c63,
    0x00006e69,
}};
