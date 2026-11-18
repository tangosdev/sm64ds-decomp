/* ov056 cartridge data at 0x02112cd0, 36 bytes. */
#include "types.h"

extern u32 data_ov056_02112bc0[];
extern u32 data_ov056_02112bc4[];
extern u32 data_ov056_02112bd8[];
extern int data_ov056_02112c04[];
extern char data_ov056_02112c68[];

u32 data_ov056_02112cd0[9] = {
    (u32)&data_ov056_02112bc0, 0x00000201, (u32)&data_ov056_02112c68, 0x00000108,
    (u32)&data_ov056_02112bd8, 0x0000010e, (u32)&data_ov056_02112bc4, 0x0000010a,
    (u32)&data_ov056_02112c04,
};
