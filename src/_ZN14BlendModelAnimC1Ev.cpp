//cpp
// @symbol _ZN14BlendModelAnimC1Ev
/* recovered: real C++ constructor. The ROM's steps: base step
 * _ZN9ModelAnimC2Ev, both vptr stores (_ZTV14BlendModelAnim primary,
 * VTable_Animation_BlendModelAnimThunk secondary at +0x50, addend 0x2c raw),
 * then blendWeight = 0x1000 and unk_6c = 0. Both are mem-initialisers in
 * declaration order; blendStep (+0x68) is deliberately left uninitialised --
 * Advance seeds it from SetAnim, and the ROM's ctor never stores it. Empty
 * body. */
#include "BlendModelAnim.h"

BlendModelAnim::BlendModelAnim() : blendWeight(0x1000), unk_6c(0) {}
