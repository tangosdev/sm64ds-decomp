/*
 * One object for data_ov006_02142050, data_ov006_02142052,
 * data_ov006_02142053 (4 bytes): the linker starts every input section on a
 * 4-byte boundary, so these tables cannot be separate.
 */
#include "types.h"

struct {
    u8 data_ov006_02142050[2];
    u8 data_ov006_02142052;
    u8 data_ov006_02142053;
} data_ov006_02142050;
