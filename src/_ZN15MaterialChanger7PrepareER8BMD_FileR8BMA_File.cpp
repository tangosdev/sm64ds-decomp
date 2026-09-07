//cpp
// @symbol _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File
#include "MaterialChanger.h"
extern "C" void func_020470e8(BMD_File *model, BMA_File *file);

/* The ROM body is a 0xc long-call veneer (ldr ip, [pc]; bx ip); the
   registers pass through untouched, so the matched func_020470e8.c's
   own two-argument signature is the call surface. Prepare is static:
   no this. */
void MaterialChanger::Prepare(BMD_File &model, BMA_File &animFile)
{
    func_020470e8(&model, &animFile);
}
