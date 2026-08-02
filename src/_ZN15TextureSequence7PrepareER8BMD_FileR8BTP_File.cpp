//cpp
// @symbol _ZN15TextureSequence7PrepareER8BMD_FileR8BTP_File
#include "TextureSequence.h"
extern "C" void func_02046d50(TextureSequence *self, BMD_File *model, BTP_File *file);

/* The ROM body is a 0xc tail-call veneer; func_02046d50 is the real,
   still unnamed implementation. */
void TextureSequence::Prepare(BMD_File &model, BTP_File &animFile)
{
    func_02046d50(this, &model, &animFile);
}
