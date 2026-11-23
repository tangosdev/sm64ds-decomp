#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
} data_ov073_021231bc_Layout;
typedef char data_ov073_021231bc_size[sizeof(data_ov073_021231bc_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
} data_ov073_021231c0_Layout;
typedef char data_ov073_021231c0_size[sizeof(data_ov073_021231c0_Layout) == 0x4 ? 1 : -1];

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
    void *f010;
    void *f014;
    void *f018;
    u32 f01c;
    void *f020;
} data_ov073_021231c4_Layout;
typedef char data_ov073_021231c4_size[sizeof(data_ov073_021231c4_Layout) == 0x24 ? 1 : -1];

extern data_ov073_021231bc_Layout data_ov073_021231bc;
extern data_ov073_021231c0_Layout data_ov073_021231c0;
extern data_ov073_021231c4_Layout data_ov073_021231c4;
extern u8 gEwbIceAModelFile[];
extern u8 gEwbIceACollisionFile[];
extern u8 data_ov057_0211153c[];
extern u8 gEwbIceBModelFile[];
extern u8 gEwbIceBCollisionFile[];
extern u8 data_ov057_0211155c[];
extern u8 gEwbIceCModelFile[];
extern u8 gEwbIceCCollisionFile[];
extern u8 data_ov057_0211159c[];
extern u8 _ZTI13daObjEwbIce_c[];

data_ov073_021231c0_Layout data_ov073_021231c0 = {
    &gEwbIceACollisionFile[0],
};

data_ov073_021231bc_Layout data_ov073_021231bc = {
    &gEwbIceAModelFile[0],
};

data_ov073_021231c4_Layout data_ov073_021231c4 = {
    &data_ov057_0211153c[0],   &gEwbIceBModelFile[0],
    &gEwbIceBCollisionFile[0], &data_ov057_0211155c[0],
    &gEwbIceCModelFile[0],     &gEwbIceCCollisionFile[0],
    &data_ov057_0211159c[0],   0x00000000,
    &_ZTI13daObjEwbIce_c[0],
};
