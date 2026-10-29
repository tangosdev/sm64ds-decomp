#include "Ov007StrokePath.h"

/* Clears a new stroke path (func_ov007_020c2c68 calls it on the 0x30-byte
 * allocation); segmentAngles and progressThresholds are left alone. The
 * parameter keeps the tree's char * spelling; `path` is the typed view. */
void func_ov007_020c29b4(char *c) {
    Ov007StrokePath *path = (Ov007StrokePath *)c;
    path->representation = 0;
    path->flags = 0;
    path->pointCount = 0;
    path->capacity = 0;
    path->segmentLengths = 0;
    path->xCoords = 0;
    path->yCoords = 0;
    path->pointStorage = 0;
    /* The clear chains reload each slot unsigned (ldrh), as in
     * func_ov007_020c26fc; reading the header's s16 directly emits ldrsh. */
    path->minPointIndices[2] = 0;
    path->minPointIndices[1] = *(u16 *)&path->minPointIndices[2];
    path->minPointIndices[0] = *(u16 *)&path->minPointIndices[1];
    path->maxPointIndices[2] = 0;
    path->maxPointIndices[1] = *(u16 *)&path->maxPointIndices[2];
    path->maxPointIndices[0] = *(u16 *)&path->maxPointIndices[1];
}
