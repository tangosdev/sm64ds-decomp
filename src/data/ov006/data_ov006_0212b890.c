/*
 * Minigame-overlay read-only table at 0x0212b890..0x0212c9f0.
 *
 * Values are the cartridge words; a word the module relocates
 * is written as its target symbol so the link re-creates it.
 */
#include "types.h"
#include "nitro/fx/fx.h"

#pragma define_section obj ".rodata.before_coin_tables" ".rodata.before_coin_tables" ".rodata.before_coin_tables" far_data
#pragma section obj begin
Vector3 sMascotWalkerOffset = { 0x00000000, 0x00008000, 0x00100000 };
#pragma section obj end
