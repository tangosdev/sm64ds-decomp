//cpp
// @symbol _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File
#include "TextureSequence.h"
extern "C" void func_02046d50(BMD_File *model, BTP_File *file);

/* The ROM body is a 0xc long-call veneer (ldr ip, [pc]; bx ip); the
   registers pass through untouched, so the matched func_02046d50.c's
   own two-argument signature is the call surface. Prepare is static:
   no this. */
void TextureSequence::Prepare(BMD_File &model, BTP_File &animFile)
{
    func_02046d50(&model, &animFile);
}
