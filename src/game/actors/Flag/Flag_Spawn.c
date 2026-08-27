// @symbol Flag_Spawn
/* The pinned compiler cannot spell the ROM's folded factory in C++: `new Flag`
 * calls the unavailable global _Znwm, while the cartridge calls
 * fBase_c::operator new.  Placement new and class-specific operator new are
 * rejected by mwccarm 2004/b56 (notes/ctor-migration.md section 5d), so this
 * remains the truthful C transcription of the compiler-generated sequence. */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"

int *Flag_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(312);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV4Flag;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
    }
    return p;
}
