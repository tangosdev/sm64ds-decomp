/* The 4x3 identity: 0x1000 down the diagonal, no translation. */
#include "math/Matrix.h"

const Matrix4x3 IDENTITY_MATRIX4X3 = {{
    0x1000,
    0,
    0,
    0,
    0x1000,
    0,
    0,
    0,
    0x1000,
    0,
    0,
    0,
}};
