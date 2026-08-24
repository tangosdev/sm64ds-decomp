//cpp
// @symbol _ZN12PiranhaPlant6RenderEv
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
#include "PiranhaPlant.h"

struct G2 { int w[2]; };
extern G2 data_ov084_02130df4;

int PiranhaPlant::Render()
{
    if (mScaleX == 0)
        return 1;
    mModelAnim.Render((Vector3 *)&mScaleX);
    if ((int)mModelAnim.file == data_ov084_02130df4.w[1])
        mModel.Render(&mPipeScale);
    return 1;
}
