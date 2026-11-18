/*
 * Minigame-overlay zero-initialised storage at 0x02141640..0x02141660.
 *
 * No cartridge payload: the claim extent is the whole proof.
 */
#include "types.h"

#pragma define_section obj ".bss" ".bss" ".bss" far_data
#pragma section obj begin
int sAmidaLaneTargets[4];
#pragma section obj end
