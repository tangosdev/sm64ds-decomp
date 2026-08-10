//cpp
// @symbol _ZN6PlayerD1Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * This file was the tree's longest-standing structor blocker, and nothing was
 * ever wrong with the source. The hand-written version had to spell out the
 * vtable store, nine member destructor calls and three array cleanups, because
 * it could not be compiled from a declaration: the compiler emits
 * `__cxa_vec_cleanup` for an array member, and the ROM's runtime helper at
 * 0x0207328c was recorded under the invented name `__destroy_arr`.
 *
 * Those are the same function. 0x0207328c disassembles to the Itanium
 * `__cxa_vec_cleanup` contract exactly -- null-destructor guard, count==0
 * early-out, then a BACKWARDS walk from count-1 calling dtor(base + i*size) --
 * and it reproduces byte-exactly from the libsupc++ source text at this file's
 * own pin, 2004/b56. Its catch island at 0x020732e8 is part of the same
 * function, its 20-byte exception descriptor appears verbatim in the ROM, and
 * its neighbour at 0x02073300 is `__cxa_vec_dtor` -- the other half of the ABI
 * pair, differing by exactly what the ABI says separates them.
 *
 * `__destroy_arr` was a bulk rename with no recorded evidence. It described
 * the role correctly and named the function wrongly. config/arm9/symbols.txt
 * now carries `__cxa_vec_cleanup` as a size-0 alias at the same address, the
 * mechanism this tree already uses for five ITCM runtime helpers: a relocation
 * needs a symbol's address, never its size, and the gap object holding this
 * address has exactly zero slack, so 0x0 is the only size that can link.
 *
 * The reverse-order member destruction below is not written anywhere any more.
 * It is Player.h's declaration order, read backwards by the compiler.
 */
#include "Player.h"

Player::~Player()
{
}
