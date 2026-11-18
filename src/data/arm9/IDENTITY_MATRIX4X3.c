/* The 4x3 identity matrix (0x1000 in each diagonal word). */
#include "types.h"

#include "common.h"

extern const Matrix4x3 IDENTITY_MATRIX4X3 = {{0x1000, 0, 0, 0, 0x1000, 0, 0, 0, 0x1000, 0, 0, 0}};
