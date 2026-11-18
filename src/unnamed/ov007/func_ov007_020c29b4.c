#include "Ov007StrokePath.h"

/* Clears a new stroke path (func_ov007_020c2c68 calls it on the 0x30-byte
 * allocation); segmentAngles and progressThresholds are left alone. */
void func_ov007_020c29b4(Ov007StrokePath *path) {
    path->representation = 0;
    path->flags = 0;
    path->pointCount = 0;
    path->capacity = 0;
    path->segmentLengths = 0;
    path->xCoords = 0;
    path->yCoords = 0;
    path->pointStorage = 0;
    path->minPointIndices[0] = path->minPointIndices[1] = path->minPointIndices[2] = 0;
    path->maxPointIndices[0] = path->maxPointIndices[1] = path->maxPointIndices[2] = 0;
}
