//cpp
// @symbol _ZN18TextureTransformer6UpdateER15ModelComponents
#include "TextureTransformer.h"
extern "C" void func_020469e8(ModelComponents *model, BTA_File *file, int frame);

void TextureTransformer::Update(ModelComponents &model)
{
    func_020469e8(&model, file, (u16)((u32)currFrame >> 12));
}
