/*
 * Minigame-overlay zero-initialised storage at 0x02141768..0x02141774.
 *
 * No cartridge payload: the claim extent is the whole proof.
 */
#include "types.h"

#pragma define_section obj ".bss" ".bss" ".bss" far_data
#pragma section obj begin
int sCardSelectLocked;
#pragma section obj end
