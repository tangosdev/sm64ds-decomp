/*
 * One object for sAmidaPatternRungSkip, sAmidaPatternExtraRungs (12 bytes):
 * the linker starts every input section on a 4-byte boundary, so these
 * tables cannot be separate.
 */
#include "types.h"

#pragma define_section obj ".rodata.before_coin_tables" ".rodata.before_coin_tables" ".rodata.before_coin_tables" far_data
#pragma section obj begin
struct {
    char sAmidaPatternRungSkip[4];
    char sAmidaPatternExtraRungs[8];
} sAmidaPatternRungSkip = {
    {
        0x01, 0x00, 0x00, 0x00,
    },
    ""
};
#pragma section obj end
