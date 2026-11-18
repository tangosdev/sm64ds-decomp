/* Word table, 3 entries. */
#include "types.h"

#pragma define_section obj ".rodata.before_coin_tables" ".rodata.before_coin_tables" ".rodata.before_coin_tables" far_data
#pragma section obj begin
int sTrmpItemPatrolLimits[3] = {
    0x00048000,
    0x00040000,
    0x00038000,
};
#pragma section obj end
