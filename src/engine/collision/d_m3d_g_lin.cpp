//cpp
/* leftovers:
 * - func_ov002_020fea68 and func_ov002_020fea4c stay extern "C"
 *   int* copies. The first argument is the caller's out-buffer, not
 *   the line, and a Vector3 copy emits a destructor. 020fea68 copies
 *   the start (words 0..2). 020fea4c copies the end (words 3..5).
 *   dBgCh_Lin calls them by those addresses.
 * - Only D2 is in the ROM. The D1 this definition also emits is
 *   deadstripped.
 *
 * dM3dGLin, a line of two endpoints. Set copies both. No vtable.
 */

#include "dM3dGLin.h"

// @symbol _ZN8dM3dGLinD2Ev
dM3dGLin::~dM3dGLin()
{
}

// @symbol _ZN8dM3dGLin3SetERK7Vector3S2_
/* A line is exactly its two endpoints. The explicit component copies
 * preserve the ROM's interleaved load/store sequence. */
void dM3dGLin::Set(const Vector3 &start_, const Vector3 &end_)
{
    start.x = start_.x;
    start.y = start_.y;
    start.z = start_.z;
    end.x = end_.x;
    end.y = end_.y;
    end.z = end_.z;
}

extern "C" {
/* The two endpoint accessors, dM3dGLin::GetStart/GetEnd in the header docs:
   each copies one Vector3 out of the line into the caller's out-param. Defined
   last so reverse source-order emission places them at the head of the run. */
// @symbol func_ov002_020fea68
void func_ov002_020fea68(int *out, int *line) {
    out[0] = line[0];
    out[1] = line[1];
    out[2] = line[2];
}

// @symbol func_ov002_020fea4c
void func_ov002_020fea4c(int *out, int *line) {
    out[0] = line[3];
    out[1] = line[4];
    out[2] = line[5];
}
}
