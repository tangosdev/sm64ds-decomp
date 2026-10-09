//cpp
// @symbol _ZN14BlendModelAnimD1Ev
/* recovered: real C++ destructor -- defined out of line, D1 bound here
 *
 * The ROM's body is the two vptr stores (_ZTV14BlendModelAnim primary,
 * VTable_Animation_BlendModelAnimThunk at +0x50), the conditional free of
 * unk_6c -- a buffer the class owns -- then the ModelAnim base step. The
 * free is written as the explicit `if` because a bare `delete` expression
 * would call the inherited operator delete unconditionally; the ROM tests
 * the pointer first (notes/dtor-migration.md section 2).
 */
#include "BlendModelAnim.h"

extern "C" void func_0203cbc0(void *ptr);

BlendModelAnim::~BlendModelAnim()
{
    if (unk_6c)
        func_0203cbc0(unk_6c);
}
