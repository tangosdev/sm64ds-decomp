//cpp
// @symbol _ZN8IceSheetD0Ev
/* A delete expression forces the genuine compiler-spelled deleting
 * destructor. dActor_c's inline operator delete supplies the actor-heap
 * deallocation used by the cartridge. */
#include "IceSheet.h"

void IceSheet_EmitDeletingDestructor(IceSheet *sheet)
{
    delete sheet;
}
