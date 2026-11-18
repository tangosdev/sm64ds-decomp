/*
 * One object for data_ov006_0213e354, data_ov006_0213e370 (32 bytes): the
 * linker starts every input section on a 4-byte boundary, so these tables
 * cannot be separate.
 */
#include "types.h"

struct {
    char data_ov006_0213e354[28];
    char data_ov006_0213e370[4];
} data_ov006_0213e354 = {{
                             0x00, 0xd0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20,
                             0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x40, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00,
                         },
                         ""};
