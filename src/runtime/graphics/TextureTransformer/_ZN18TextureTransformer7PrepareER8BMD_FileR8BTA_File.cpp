//cpp
// @symbol _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File
#include "TextureTransformer.h"
extern "C" void func_02046b64(BMD_File *model, BTA_File *file);

/* The ROM body is a 0xc long-call veneer (ldr ip, [pc]; bx ip); the
   registers pass through untouched, so the matched func_02046b64.c's
   own two-argument signature is the call surface. Prepare is static:
   no this. */
void TextureTransformer::Prepare(BMD_File &model, BTA_File &animFile)
{
    func_02046b64(&model, &animFile);
}
