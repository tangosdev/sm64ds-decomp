#ifndef ENDINGFLYPOINT_H
#define ENDINGFLYPOINT_H

/* One control point of gEndingFlyPath, the path St_EndingFly_Main flies the
   player along: the speed at which the spline advances past this point, in
   thousandths of a span per frame, then its position. */
struct EndingFlyPoint {
    short speed;
    short x;
    short y;
    short z;
};

#endif
