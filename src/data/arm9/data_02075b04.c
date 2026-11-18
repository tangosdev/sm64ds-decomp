/* The identity quaternion (0, 0, 0, 1.0) Quaternion_FromVector3 returns
   when the cross product is degenerate. */
#include "types.h"

typedef struct {
    int x, y, z, w;
} Quat;

extern const Quat data_02075b04 = {0, 0, 0, 0x1000};
