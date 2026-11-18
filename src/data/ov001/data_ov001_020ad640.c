/*
 * Title-overlay BSS tail.
 *
 * Two records no matched function addresses yet; they sit between CapIconMgr's
 * bss claim and the end of the module image.
 */
#include "types.h"

#pragma define_section obj ".bss" ".bss" ".bss" far_data
#pragma section obj begin
u32 data_ov001_020ad640;
#pragma section obj end
