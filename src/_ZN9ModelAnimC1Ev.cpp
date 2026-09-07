//cpp
// @symbol _ZN9ModelAnimC1Ev
/* recovered: real C++ constructor. The ROM's five steps are base step
 * _ZN5ModelC2Ev, base step _ZN9AnimationC2Ev at +0x50, the primary vptr store
 * (_ZTV9ModelAnim), the secondary vptr store landing on
 * VTable_Animation_ModelAnimThunk (+0x24 into the vtable object, addend 0x2c
 * raw), and the body store file = 0. Declaring both bases' ctors and this
 * class's ctor makes synthesis emit all of it in that order; the body is the
 * one store. The C2 sibling this TU emits is stripped by objisolate;
 * src/_ZN9ModelAnimC2Ev.cpp binds the same source-level constructor to that
 * enrolled base-object variant. */
#include "ModelAnim.h"

ModelAnim::ModelAnim()
{
    file = 0;
}
