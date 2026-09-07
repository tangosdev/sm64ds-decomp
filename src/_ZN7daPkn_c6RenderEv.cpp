//cpp
// @symbol _ZN7daPkn_c6RenderEv
/* recovered: named members + shared header, real C++ method
 *
 * The six-slot `struct Obj` this file used to cast both models to was their own
 * vtable, and the slot it called is Render -- ModelAnim's for the plant, Model's
 * for the pipe it sits in.
 *
 * `mModelAnim.file` is a BCA_File * where the raw field was an int, so the
 * comparison against the second word of data_ov084_02130df4 needs the cast the
 * offset spelling did not.
 */
#include "daPkn_c.h"

struct G2 { int w[2]; };
extern G2 data_ov084_02130df4;

int daPkn_c::Render()
{
    if (mScaleX == 0)
        return 1;
    mModelAnim.Render((Vector3 *)&mScaleX);
    if ((int)mModelAnim.file == data_ov084_02130df4.w[1])
        mModel.Render(&mPipeScale);
    return 1;
}
