#ifndef OV007_STROKE_PATH_H
#define OV007_STROKE_PATH_H

#include "types.h"

/* Reconstructed names and layout, not an original class identity. */
typedef struct Ov007StrokePath {
    int representation;
    int flags;
    u16 pointCount;
    u16 capacity;
    /* X/Y extrema store point indices, not coordinates. The third slots are
     * only known to be cleared; signedness is not uniquely recoverable. */
    s16 minPointIndices[3];
    s16 maxPointIndices[3];
    int *segmentLengths;
    u16 *segmentAngles;
    int *progressThresholds;
    int *xCoords; /* Q12 coordinates in planar representation */
    int *yCoords;
    void *pointStorage; /* alternate representation: 12 bytes per point */
} Ov007StrokePath;

#ifndef SM64DS_PLATFORM_PC
typedef char Ov007StrokePath_size_must_be_48[sizeof(Ov007StrokePath) == 0x30 ? 1 : -1];
#endif

enum { OV007_PATH_PLANAR = 0, OV007_PATH_HAS_PROGRESS = 1 };

/* The builders: an empty planar path (func_ov007_020c2b9c), then one filled
 * from int or byte coordinate tables. */
#ifdef __cplusplus
extern "C" {
#endif

Ov007StrokePath *func_ov007_020c2acc(int pointCount, const unsigned char *xCoords,
                                     const unsigned char *yCoords, const int *progressThresholds);
Ov007StrokePath *func_ov007_020c2b38(int pointCount, const int *xCoords, const int *yCoords,
                                     const int *progressThresholds);
Ov007StrokePath *func_ov007_020c2b9c(int pointCount, const int *progressThresholds);

#ifdef __cplusplus
}
#endif

#endif
