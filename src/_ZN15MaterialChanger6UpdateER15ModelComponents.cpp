//cpp
// @symbol _ZN15MaterialChanger6UpdateER15ModelComponents
#include "MaterialChanger.h"
extern "C" void func_02046e28(ModelComponents *model, BMA_File *file, int frame);

void MaterialChanger::Update(ModelComponents &model)
{
    func_02046e28(&model, file, (u16)((u32)currFrame >> 12));
}
