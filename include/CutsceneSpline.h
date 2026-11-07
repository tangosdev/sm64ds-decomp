#ifndef CUTSCENESPLINE_H
#define CUTSCENESPLINE_H

#include "types.h"

struct Vector3;

/* One control point of a camera path: Super Mario 64's CutsceneSplinePoint.
   index is ignored except that a negative one ends the path. speed roughly
   sets how many frames a segment takes; MovePointAlongSpline blends the
   speeds of the segment's two inner points. A path whose speeds are all 0
   never advances the progress it shares with the other path of its pair. */
struct CutsceneSplinePoint {
    s8 index;
    u8 speed;
    s16 x;
    s16 y;
    s16 z;
};

/* A path as the cutscene scripts name it: the point count, then the points. */
struct CutsceneSpline {
    u8 numPoints;
    CutsceneSplinePoint *points;
};

/* Move pos along spline: the uniform cubic B-spline through the four points
   from *segment, at *progress, which advances by the points' speeds and
   carries into the next segment. Returns 1 once the path has run out. */
/* Parameters: pos, segment, progress, spline. They stay unnamed because a
   named parameter in a declaration uses up a compiler number (@NNN, $NNN)
   in every file that includes this header. */
int MovePointAlongSpline(Vector3 *, s16 *, s32 *, CutsceneSpline *);

#endif
