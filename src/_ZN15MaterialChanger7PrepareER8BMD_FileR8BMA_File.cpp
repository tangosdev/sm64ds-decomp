//cpp
// @symbol _ZN15MaterialChanger7PrepareER8BMD_FileR8BMA_File
#include "MaterialChanger.h"
extern "C" void func_020470e8(MaterialChanger *self, BMD_File *model, BMA_File *file);

/* The ROM body is a 0xc tail-call veneer; func_020470e8 is the real,
   still unnamed implementation. */
void MaterialChanger::Prepare(BMD_File &model, BMA_File &animFile)
{
    func_020470e8(this, &model, &animFile);
}
