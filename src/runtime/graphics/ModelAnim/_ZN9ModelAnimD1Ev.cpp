//cpp
// @symbol _ZN9ModelAnimD1Ev
/* recovered: real C++ destructor -- defined out of line, D1 bound here
 *
 * ModelAnim.h declares `virtual ~ModelAnim();` but defines it nowhere, so
 * this TU's empty body is what emits the symbol. The compiler then writes
 * the two vptr stores (_ZTV9ModelAnim primary, VTable_Animation_ModelAnimThunk
 * at +0x50), runs the Animation member step and the Model base step -- the
 * same body the ROM carries at 0x0201691c. objisolate keeps only the D1
 * variant this file is bound to; the C2/D0 siblings stay in their own files
 * (notes/dtor-migration.md section 1).
 */
#include "ModelAnim.h"

ModelAnim::~ModelAnim()
{
}
