//cpp
/* dCapEnemy_c -- the ov002 half of the class.
 *
 * ov002/dCapEnemy_c  (2 function(s))  .text 0x020aedbc .. 0x020aee40
 *
 * dCapEnemy_c's members do NOT all live in one module. Ten ordinary members,
 * the constructor and D1 are in arm9 (0x02005d94 .. 0x02006588); only the
 * base-object destructor D2 (0x020aedbc) and the deleting destructor D0
 * (0x020aedf4) are here in ov002. This file is therefore the ov002 half only.
 *
 * FUNCTION ORDER IS DELIBERATELY THE REVERSE OF THE ROM'S -- mwccarm 2004/b56
 * emits one .text section per function in the REVERSE of source order, so the
 * highest-address ROM function is written first. A destructor's D0/D1/D2 group
 * is the documented exception: they come from ONE definition and the compiler
 * chooses their relative order.
 *
 * D0 has no source of its own. mwccarm emits D1, D2 and D0 from the single
 * out-of-line `dCapEnemy_c::~dCapEnemy_c() {}` below; there is no second
 * definition to write and writing one is a redefinition error. D0's @symbol
 * marker is therefore parked at its ROM ordinal above this note, so that every
 * member defined in this TU carries a marker.
 */
#include "dCapEnemy_c.h"

/* -------------------------------------------------------------------------- */
/* ROM ordinal 1 -- _ZN11dCapEnemy_cD0Ev, 0x020aedf4, size 0x4c */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dCapEnemy_cD0Ev
/* The DELETING destructor: destroy through this class and its bases -- which is
 * why more than one vptr store appears -- then return the object to its heap.
 * Nobody writes that. Declaring `~dCapEnemy_c()` is enough; the deallocation is
 * an inline operator delete, which is why nothing here mentions a heap.
 *
 * No body: emitted by the definition below. See the file header.
 */

/* -------------------------------------------------------------------------- */
/* ROM ordinal 0 -- _ZN11dCapEnemy_cD2Ev, 0x020aedbc, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN11dCapEnemy_cD2Ev
/* recovered: real C++ destructor -- the compiler emits the whole body
 *
 * The base-object variant, and the one every derived cap enemy chains to.
 * It is byte-identical to D1 at arm9 0x0200651c -- dCapEnemy_c has no virtual
 * bases -- modulo the three relocated `bl` words, which is why this address
 * spent so long carrying the placeholder name func_ov002_020aedbc while arm9
 * 0x0200651c wore the D2 name that belongs here.
 *
 * Which is which is settled by how the ROM reaches them, not by their bytes:
 * word 16 of _ZTV11dCapEnemy_c (0x021082c4) holds 0x0200651c, so that one is
 * D1, and word 17 holds 0x020aedf4, which is D0. This one has no vtable word
 * at all and is reached only by `bl` from daTrs_c's and daKrb_c's destructors
 * tearing down their base sub-object, which is exactly what D2 is for.
 */
dCapEnemy_c::~dCapEnemy_c()
{
}
