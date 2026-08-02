//cpp
// @symbol _ZN18TextureTransformer7PrepareER8BMD_FileR8BTA_File
#include "TextureTransformer.h"
extern "C" void func_02046b64(TextureTransformer *self, BMD_File *model, BTA_File *file);

/* The ROM body is a 0xc tail-call veneer; func_02046b64 is the real,
   still unnamed implementation. */
void TextureTransformer::Prepare(BMD_File &model, BTA_File &animFile)
{
    func_02046b64(this, &model, &animFile);
}
