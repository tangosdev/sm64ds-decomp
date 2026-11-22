//cpp
#include "dCamera_c.h"
#include "math/CalcLib.h"

extern struct CamMode data_020874cc;
extern struct CamMode data_020874f4;

/* Camera state main: while the mode is data_020874cc, ease mBaseAngleOffset
 * (seeded by func_02005060) back to 0, and switch to data_020874f4 once it is
 * there. Then run the shared camera behaviour, func_02009e70. */
void dCamera_c::func_02005000() {
    if (this->mMode == &data_020874cc) {
        if (!ApproachAngle(&this->mBaseAngleOffset, 0, 8, 0x2000, 0x400))
            this->mMode = &data_020874f4;
    }
    this->func_02009e70();
}
