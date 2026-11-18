/* 16-bit table, 16 entries. */
#include "types.h"

#pragma define_section obj ".bss" ".bss" ".bss" far_data
#pragma section obj begin
unsigned short sHanaUsedIds[16];
#pragma section obj end
