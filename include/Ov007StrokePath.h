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

typedef char Ov007StrokePath_size_must_be_48[sizeof(Ov007StrokePath) == 0x30 ? 1 : -1];

enum { OV007_PATH_PLANAR = 0, OV007_PATH_HAS_PROGRESS = 1 };

/* Keep the existing opaque pointer interface while the remaining engine
 * consumers are reconstructed. These entry points retain their C ABI. */
#ifdef __cplusplus
extern "C" {
#endif

void *func_ov007_020c2acc(int pointCount, const unsigned char *xCoords,
                          const unsigned char *yCoords, const int *progressThresholds);
void *func_ov007_020c2b38(int pointCount, const int *xCoords, const int *yCoords,
                          const int *progressThresholds);
void *func_ov007_020c2b9c(int pointCount, const int *progressThresholds);

#ifdef __cplusplus
}
#endif

#endif
