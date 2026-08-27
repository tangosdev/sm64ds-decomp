//cpp
// @symbol _ZN15TextureSequence6UpdateER15ModelComponents
#include "TextureSequence.h"
extern "C" void func_02046bbc(ModelComponents *model, BTP_File *file, int frame);

void TextureSequence::Update(ModelComponents &model)
{
    func_02046bbc(&model, file, (u16)((u32)currFrame >> 12));
}
